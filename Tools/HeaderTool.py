from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path

HEADER_EXTS = {".h", ".hpp", ".hh"}
GENERATED_SUFFIX = ".generated.h"
MANIFEST_NAME = "reflectManifest.h"
MARKER = "// reflect: "
CLASS_MACRO = "TCLASS"

_NOISE = re.compile(
    r"""//[^\n]*"""
    r"""|/\*.*?\*/"""
    r"""|R"(?P<d>[^(\s]*)\(.*?\)(?P=d)\""""
    r"""|"(?:\\.|[^"\\\n])*\""""
    r"""|(?<![0-9A-Za-z_])'(?:\\.[^'\n]*|[^'\\\n])'""",
    re.S,
)


def strip_noise(text: str) -> str:
    def repl(m: re.Match) -> str:
        s = m.group(0)
        if s.startswith("/"):
            return " "
        return '""'

    return _NOISE.sub(repl, text)


_CLASS = (
    r"(?P<cc>\b" + CLASS_MACRO + r"\s*\([^)]*\)\s*(?P<kw>class|struct)\s+"
    r"(?:\[\[[^\]]*\]\]\s*)*"
    r"(?:[A-Z0-9_]*(?:API|EXPORT)[A-Z0-9_]*\s+)?"
    r"(?P<name>[A-Za-z_]\w*)\s*(?:final\s*)?"
    r"(?::(?P<bases>[^{;]*))?\{)"
)
_NAMESPACE = r"(?P<ns>\bnamespace\b\s*(?P<nsname>[A-Za-z_][\w:\s]*?)?\s*\{)"
_TOKEN = re.compile("|".join([_CLASS, _NAMESPACE, r"(?P<open>\{)", r"(?P<close>\})"]))


@dataclass
class ClassInfo:
    name: str
    namespace: str
    scoped: str  # Outer::Inner (no namespace)
    base: str | None

    @property
    def qualified(self) -> str:
        """Fully qualified name, with a leading '::'."""
        return f"::{self.namespace}::{self.scoped}" if self.namespace else f"::{self.scoped}"


def pick_base(bases: str | None, kw: str) -> str | None:
    if not bases or not bases.strip():
        return None
    parts, depth, cur = [], 0, ""
    for ch in bases:
        if ch in "<(":
            depth += 1
        elif ch in ">)":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    parts.append(cur)

    for p in parts:
        p = re.sub(r"\bvirtual\b", "", p).strip()
        m = re.match(r"(public|protected|private)\s+(.*)$", p, re.S)
        access = m.group(1) if m else ("public" if kw == "struct" else "private")
        if access == "public":
            return re.sub(r"\s+", " ", (m.group(2) if m else p).strip())
    return None


def parse_header(text: str) -> list[ClassInfo]:
    text = strip_noise(text)
    stack: list[tuple[str, str]] = []
    found: list[ClassInfo] = []

    for m in _TOKEN.finditer(text):
        if m.group("cc"):
            name = m.group("name")
            ns = "::".join(n for k, n in stack if k == "ns" and n)
            outer = [n for k, n in stack if k == "cls"]
            found.append(
                ClassInfo(
                    name=name,
                    namespace=ns,
                    scoped="::".join(outer + [name]),
                    base=pick_base(m.group("bases"), m.group("kw")),
                )
            )
            stack.append(("cls", name))
        elif m.group("ns"):
            nsname = re.sub(r"\s+", "", m.group("nsname") or "")
            stack.append(("ns", nsname))
        elif m.group("open"):
            stack.append(("blk", ""))
        elif m.group("close"):
            if stack:
                stack.pop()
    return found


def render_generated_header(header_inc: str, c: ClassInfo, registry_include: str, registry_ns: str) -> str:
    """Emits a TypeAccessor<T> specialization that builds the type with TypeBuilder."""
    out = [
        "#pragma once",
        "",
        f"{MARKER}{c.qualified}",
        "",
        f"#include <{registry_include}>",
        f"#include <{header_inc}>",
        "",
        f"namespace {registry_ns} {{",
        "\ttemplate<>",
        f"\tstruct TypeAccessor<{c.qualified}> {{",
        "\t\tstatic Type Build() {",
    ]
    if c.namespace:
        out.append(f"\t\t\tusing namespace {c.namespace};")
        out.append("")
    out.append(f'\t\t\treturn TypeBuilder<{c.qualified}>::ForType("{c.name}")')
    if c.base:
        out.append(f"\t\t\t\t.Base<{c.base}>()")
    out += [
        "\t\t\t\t.Build();",
        "\t\t}",
        "\t};",
        "}",
        "",
    ]
    return "\n".join(out)


def collect_entries(inter: Path) -> list[tuple[str, str, str]]:
    entries = []
    for f in inter.glob("*/*" + GENERATED_SUFFIX):
        try:
            text = f.read_text(encoding="utf-8")
        except OSError as e:
            print(f"HeaderTool: warning: skipping {f}: {e}", file=sys.stderr)
            continue
        m = re.search(r"^" + re.escape(MARKER) + r"(\S+)\s*$", text, re.M)
        if not m:
            continue
        entries.append((f.parent.name, f.name, m.group(1)))
    entries.sort()
    return entries


def render_manifest(
    entries: list[tuple[str, str, str]],
    registry_include: str,
    registry_ns: str,
    export_macro: str,
) -> str:
    out = [
        "#pragma once",
        "",
        "#include <vector>",
        f"#include <{registry_include}>",
        "",
    ]
    out += [f'#include "{proj}/{name}"' for proj, name, _ in entries]
    out += [
        "",
        f'extern "C" {export_macro} void GenerateModuleManifestation(std::vector<{registry_ns}::Type>* outTypes) {{'.replace("  ", " "),
        "\tif (!outTypes)",
        "\t\treturn;",
        "",
        f"\tusing namespace {registry_ns};",
        "",
    ]
    last_project = None
    for proj, _, qualified in entries:
        if proj != last_project:
            if last_project is not None:
                out.append("")
            out.append(f"\t// {proj}")
            last_project = proj
        out.append(f"\toutTypes->push_back(TypeAccessor<{qualified}>::Build());")
    out += ["}", ""]
    return "\n".join(out)


def write_if_changed(path: Path, content: str) -> bool:
    data = content.encode("utf-8")
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists() and path.read_bytes() == data:
        return False
    path.write_bytes(data)
    return True


def main() -> int:
    ap = argparse.ArgumentParser(description="Generate reflection headers and manifest.")
    ap.add_argument("--root", required=True)
    ap.add_argument("--project", required=True)
    ap.add_argument("--source", required=True)
    ap.add_argument("--include-base")
    ap.add_argument("--include-prefix")
    ap.add_argument("--intermediate", default="Intermative")
    ap.add_argument("--registry-include", default="Runtime/Reflection/TypeBuilder.h")
    ap.add_argument("--registry-namespace", default="Runtime::Reflection")
    ap.add_argument("--export-macro", help="defaults to <PROJECT>_API")
    args = ap.parse_args()

    root = Path(args.root).resolve()
    source = Path(args.source)
    source = (source if source.is_absolute() else root / source).resolve()
    include_base = Path(args.include_base) if args.include_base else source
    include_base = (include_base if include_base.is_absolute() else root / include_base).resolve()
    inter = (root / args.intermediate).resolve()
    proj_dir = inter / args.project
    include_prefix = (args.project if args.include_prefix is None else args.include_prefix).strip("/")
    export_macro = args.export_macro if args.export_macro is not None else f"{args.project.upper()}_API"

    if not source.is_dir():
        print(f"HeaderTool: source folder not found: {source}", file=sys.stderr)
        return 1

    headers = sorted(
        p
        for p in source.rglob("*")
        if p.suffix.lower() in HEADER_EXTS
        and not p.name.endswith(GENERATED_SUFFIX)
        and inter not in p.parents
    )

    expected: dict[str, Path] = {}
    changed = 0
    errors = 0

    for h in headers:
        raw = h.read_text(encoding="utf-8", errors="replace")
        if CLASS_MACRO not in raw:
            continue
        classes = parse_header(raw)
        if not classes:
            continue

        try:
            include = h.relative_to(include_base).as_posix()
        except ValueError:
            include = h.name
        if include_prefix:
            include = f"{include_prefix}/{include}"

        for c in classes:
            gen_name = c.name + GENERATED_SUFFIX
            if gen_name in expected:
                print(
                    f"HeaderTool: error: class '{c.name}' is declared in both {h} and "
                    f"{expected[gen_name]}; reflected class names must be unique per project",
                    file=sys.stderr,
                )
                errors += 1
                continue
            expected[gen_name] = h
            changed += write_if_changed(
                proj_dir / gen_name,
                render_generated_header(include, c, args.registry_include, args.registry_namespace),
            )

    if errors:
        return 1

    if proj_dir.is_dir():
        for stale in proj_dir.glob("*" + GENERATED_SUFFIX):
            if stale.name not in expected:
                stale.unlink()
                changed += 1
        for legacy in proj_dir.glob("*.reflect.json"):
            legacy.unlink()
            changed += 1

    changed += write_if_changed(
        inter / MANIFEST_NAME,
        render_manifest(collect_entries(inter), args.registry_include, args.registry_namespace, export_macro),
    )

    print(
        f"HeaderTool: {args.project}: {len(expected)} class(es), {changed} file(s) written/removed"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
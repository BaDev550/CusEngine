#!/usr/bin/env python3
"""
reflect_gen.py - tiny reflection code generator (no libclang needed).

Scans a project's headers for

    CCLASS()
    class ENGINE_API Texture2D final : public Asset { GENERATE_CLASS(Texture2D) ... };

User headers never include anything generated. For every CCLASS() class this writes

    <ROOT>/Intermative/<Project>/<ClassName>.generated.h

which includes the class's own header and declares a type alias in the class's namespace:

    namespace CusEngine {
        using Texture2D_Reflected = ::Runtime::Reflection::Reflected<Texture2D, Asset>;
    }

(the base is written inside the class's namespace, so it resolves exactly as in the source).
Then it rebuilds

    <ROOT>/Intermative/ReflectManifest.h

which includes every project's generated headers and lists all aliases in one
Runtime::Reflection::TypeList. No state file is kept: the manifest is rebuilt by reading the
`// reflect:` marker line of every *.generated.h found under the intermediate folder.

Usage:
    python reflect_gen.py --root <ROOT> --project Engine --source Source/Engine

Run it once per project. Files are only rewritten when their content changes, so builds
are not retriggered.
"""
from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path

HEADER_EXTS = {".h", ".hpp", ".hh"}
GENERATED_SUFFIX = ".generated.h"
MARKER = "// reflect: "

# --------------------------------------------------------------------------- #
# Source cleaning + parsing
# --------------------------------------------------------------------------- #

_NOISE = re.compile(
    r"""//[^\n]*"""                                   # line comment
    r"""|/\*.*?\*/"""                                 # block comment
    r"""|R"(?P<d>[^(\s]*)\(.*?\)(?P=d)\""""           # raw string
    r"""|"(?:\\.|[^"\\\n])*\""""                      # string
    r"""|(?<![0-9A-Za-z_])'(?:\\.[^'\n]*|[^'\\\n])'""",  # char literal
    re.S,
)


def strip_noise(text: str) -> str:
    """Blank out comments and string/char literals so braces inside them don't count."""

    def repl(m: re.Match) -> str:
        s = m.group(0)
        if s.startswith("/"):
            return " "
        return '""'

    return _NOISE.sub(repl, text)


_CLASS = (
    r"(?P<cc>\bCCLASS\s*\([^)]*\)\s*(?P<kw>class|struct)\s+"
    r"(?:\[\[[^\]]*\]\]\s*)*"
    r"(?:[A-Z0-9_]*(?:API|EXPORT)[A-Z0-9_]*\s+)?"
    r"(?P<name>[A-Za-z_]\w*)\s*(?:final\s*)?"
    r"(?::(?P<bases>[^{;]*))?\{)"
)
_NAMESPACE = r"(?P<ns>\bnamespace\b\s*(?P<nsname>[A-Za-z_][\w:\s]*?)?\s*\{)"
_TOKEN = re.compile("|".join([_CLASS, _NAMESPACE, r"(?P<open>\{)", r"(?P<close>\})"]))


@dataclass
class ClassInfo:
    name: str          # Texture2D
    namespace: str     # CusEngine::Streaming (may be empty)
    scoped: str        # Texture2D, or Outer::Inner for nested classes
    base: str | None   # as written in the source, or None for a root class

    @property
    def alias(self) -> str:
        return self.scoped.replace("::", "_") + "_Reflected"

    @property
    def qualified_alias(self) -> str:
        return f"{self.namespace}::{self.alias}" if self.namespace else self.alias


def pick_base(bases: str | None, kw: str) -> str | None:
    """First publicly inherited base, as written. Template args are kept intact."""
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
    stack: list[tuple[str, str]] = []  # ("ns" | "cls" | "blk", name)
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


# --------------------------------------------------------------------------- #
# Rendering
# --------------------------------------------------------------------------- #

BANNER = "// <auto-generated> by reflect_gen.py - DO NOT EDIT"


def render_generated_header(header_rel: str, c: ClassInfo, registry_include: str, registry_ns: str) -> str:
    """<ClassName>.generated.h: includes the source header, declares the Reflected alias."""
    alias_decl = (
        f"using {c.alias} = ::{registry_ns}::Reflected<{c.scoped}, {c.base or 'void'}>;"
    )
    out = [
        "#pragma once",
        "",
        BANNER,
        f"// Source: {header_rel}",
        f"{MARKER}{c.qualified_alias}",
        "",
        f"#include <{registry_include}>",
        f"#include <{header_rel}>",
        "",
    ]
    if c.namespace:
        out += [f"namespace {c.namespace} {{", f"\t{alias_decl}", "}"]
    else:
        out += [alias_decl]
    out.append("")
    return "\n".join(out)


def collect_entries(inter: Path) -> list[tuple[str, str, str]]:
    """(project, generated file name, qualified alias) for every generated header on disk."""
    entries = []
    for f in inter.glob("*/*" + GENERATED_SUFFIX):
        try:
            text = f.read_text(encoding="utf-8")
        except OSError as e:
            print(f"reflect_gen: warning: skipping {f}: {e}", file=sys.stderr)
            continue
        m = re.search(r"^" + re.escape(MARKER) + r"(\S+)\s*$", text, re.M)
        if not m:
            continue
        entries.append((f.parent.name, f.name, m.group(1)))
    entries.sort()
    return entries


def render_manifest(entries: list[tuple[str, str, str]], registry_include: str, registry_ns: str) -> str:
    out = [
        "#pragma once",
        "",
        BANNER,
        "",
        f"#include <{registry_include}>",
    ]
    out += [f'#include "{proj}/{name}"' for proj, name, _ in entries]
    out += ["", f"namespace {registry_ns} {{", "\tusing GeneratedTypes = TypeList<"]
    last_project = None
    for i, (proj, _, alias) in enumerate(entries):
        if proj != last_project:
            out.append(f"\t\t// {proj}")
            last_project = proj
        out.append(f"\t\t{alias}" + ("," if i + 1 < len(entries) else ""))
    out += [
        "\t>;",
        "",
        "\tinline void RegisterGeneratedTypes(TypeRegistry& r) {",
        "\t\tGeneratedTypes::RegisterAll(r);",
        "\t}",
        "}",
        "",
    ]
    return "\n".join(out)


def write_if_changed(path: Path, content: str) -> bool:
    data = content.encode("utf-8")
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists() and path.read_bytes() == data:
        return False
    path.write_bytes(data)
    return True


# --------------------------------------------------------------------------- #
# Driver
# --------------------------------------------------------------------------- #


def main() -> int:
    ap = argparse.ArgumentParser(description="Generate reflection headers and manifest.")
    ap.add_argument("--root", required=True, help="ROOTFILE: folder that contains the intermediate dir")
    ap.add_argument("--project", required=True, help="project name, e.g. Engine")
    ap.add_argument("--source", required=True, help="folder to scan for headers (absolute or relative to --root)")
    ap.add_argument("--include-base", help="folder that headers are included relative to (default: --source)")
    ap.add_argument(
        "--include-prefix",
        help="prefix put in front of every #include path (default: the project name; pass '' for none)",
    )
    ap.add_argument("--intermediate", default="Intermative", help="intermediate folder name under --root")
    ap.add_argument("--registry-include", default="Runtime/Reflection/TypeRegistry.h")
    ap.add_argument("--registry-namespace", default="Runtime::Reflection")
    args = ap.parse_args()

    root = Path(args.root).resolve()
    source = Path(args.source)
    source = (source if source.is_absolute() else root / source).resolve()
    include_base = Path(args.include_base) if args.include_base else source
    include_base = (include_base if include_base.is_absolute() else root / include_base).resolve()
    inter = (root / args.intermediate).resolve()
    proj_dir = inter / args.project
    include_prefix = (args.project if args.include_prefix is None else args.include_prefix).strip("/")

    if not source.is_dir():
        print(f"reflect_gen: source folder not found: {source}", file=sys.stderr)
        return 1

    headers = sorted(
        p
        for p in source.rglob("*")
        if p.suffix.lower() in HEADER_EXTS
        and not p.name.endswith(GENERATED_SUFFIX)
        and inter not in p.parents
    )

    expected: dict[str, Path] = {}  # generated file name -> source header
    changed = 0
    errors = 0

    for h in headers:
        raw = h.read_text(encoding="utf-8", errors="replace")
        if "CCLASS" not in raw:
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
                    f"reflect_gen: error: class '{c.name}' is declared in both {h} and "
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

    # Remove generated headers whose source no longer has CCLASS (or was deleted),
    # plus the .reflect.json state files older versions of this script wrote.
    if proj_dir.is_dir():
        for stale in proj_dir.glob("*" + GENERATED_SUFFIX):
            if stale.name not in expected:
                stale.unlink()
                changed += 1
        for legacy in proj_dir.glob("*.reflect.json"):
            legacy.unlink()
            changed += 1

    changed += write_if_changed(
        inter / "ReflectManifest.h",
        render_manifest(collect_entries(inter), args.registry_include, args.registry_namespace),
    )

    print(
        f"reflect_gen: {args.project}: {len(expected)} class(es), {changed} file(s) written/removed"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
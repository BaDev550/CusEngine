import sys, re, os

def generate_reflection(header_path, output_path, source_dir):
    with open(header_path, 'r', encoding='utf-8') as f:
        content = f.read()

    class_match = re.search(r'CUS_CLASS\(\)\s*class\s+(?:[A-Z0-9_]+\s+)?(\w+)', content)
    
    if not class_match:
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write("// No reflection target found\n")
        return

    class_name = class_match.group(1)
    props = re.findall(r'CUS_PROP\(\)[^;]+?(\w+)\s*(?:=.*?)?;', content)

    abs_header = os.path.abspath(header_path).replace('\\', '/')

    out_code = f'#include "{abs_header}"\n'
    out_code += f'#include <Reflection/ReflectionMacros.h>\n\n' 
    
    out_code += f'BEGIN_REFLECT({class_name})\n'
    for prop in props:
        out_code += f'    REFLECT_PROPERTY({class_name}, {prop})\n'
    out_code += f'END_REFLECT({class_name})\n'

    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(out_code)

if __name__ == "__main__":
    generate_reflection(sys.argv[1], sys.argv[2], sys.argv[3])
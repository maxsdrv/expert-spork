#!/usr/bin/env python3
"""
Generate Pydantic models from protobuf definitions
Similar to OpenAPI model generation but for protobuf files
"""

import os
import re
import sys
from pathlib import Path
from typing import Dict, List, Set


def parse_proto_file(proto_path: str) -> Dict:
    """Parse a .proto file and extract messages, enums, and services"""
    
    with open(proto_path, 'r') as f:
        content = f.read()
    
    # Remove comments
    content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    
    result = {
        'package': '',
        'imports': [],
        'enums': {},
        'messages': {},
        'services': {}
    }
    
    # Extract package
    package_match = re.search(r'package\s+([^;]+);', content)
    if package_match:
        result['package'] = package_match.group(1).strip()
    
    # Extract imports
    import_matches = re.findall(r'import\s+"([^"]+)";', content)
    result['imports'] = import_matches
    
    # Extract enums
    enum_pattern = r'enum\s+(\w+)\s*\{([^}]+)\}'
    for enum_match in re.finditer(enum_pattern, content):
        enum_name = enum_match.group(1)
        enum_body = enum_match.group(2)
        
        enum_values = {}
        value_pattern = r'(\w+)\s*=\s*(\d+);'
        for value_match in re.finditer(value_pattern, enum_body):
            value_name = value_match.group(1)
            value_num = int(value_match.group(2))
            enum_values[value_name] = value_num
        
        result['enums'][enum_name] = enum_values
    
    # Extract messages
    message_pattern = r'message\s+(\w+)\s*\{([^}]+(?:\{[^}]*\}[^}]*)*)\}'
    for message_match in re.finditer(message_pattern, content):
        message_name = message_match.group(1)
        message_body = message_match.group(2)
        
        fields = {}
        field_pattern = r'(optional|repeated|required)?\s*(\w+(?:\.\w+)*)\s+(\w+)\s*=\s*(\d+)(?:\s*\[([^\]]+)\])?;'
        
        for field_match in re.finditer(field_pattern, message_body):
            modifier = field_match.group(1) or ''
            field_type = field_match.group(2)
            field_name = field_match.group(3)
            field_number = int(field_match.group(4))
            field_options = field_match.group(5) or ''
            
            fields[field_name] = {
                'type': field_type,
                'number': field_number,
                'modifier': modifier,
                'options': field_options
            }
        
        result['messages'][message_name] = fields
    
    # Extract services (basic parsing)
    service_pattern = r'service\s+(\w+)\s*\{([^}]+)\}'
    for service_match in re.finditer(service_pattern, content):
        service_name = service_match.group(1)
        service_body = service_match.group(2)
        
        methods = {}
        method_pattern = r'rpc\s+(\w+)\s*\(([^)]+)\)\s*returns\s*\(([^)]+)\)'
        
        for method_match in re.finditer(method_pattern, service_body):
            method_name = method_match.group(1)
            input_type = method_match.group(2).strip()
            output_type = method_match.group(3).strip()
            
            # Handle stream types
            if 'stream' in input_type:
                input_type = input_type.replace('stream', '').strip()
                input_stream = True
            else:
                input_stream = False
                
            if 'stream' in output_type:
                output_type = output_type.replace('stream', '').strip()
                output_stream = True
            else:
                output_stream = False
            
            methods[method_name] = {
                'input': input_type,
                'output': output_type,
                'input_stream': input_stream,
                'output_stream': output_stream
            }
        
        result['services'][service_name] = methods
    
    return result


def proto_type_to_python(proto_type: str, is_optional: bool = False) -> str:
    """Convert protobuf type to Python type hint"""
    
    type_map = {
        'string': 'str',
        'int32': 'int',
        'int64': 'int',
        'uint32': 'int',
        'uint64': 'int',
        'bool': 'bool',
        'float': 'float',
        'double': 'float',
        'bytes': 'bytes',
    }
    
    # Handle message types (start with uppercase)
    if proto_type in type_map:
        python_type = type_map[proto_type]
    elif proto_type.startswith('google.protobuf.'):
        # Handle well-known types
        if 'Empty' in proto_type:
            python_type = 'Dict[str, Any]'
        else:
            python_type = 'Dict[str, Any]'
    else:
        # Assume it's a custom message or enum
        python_type = proto_type.split('.')[-1]  # Get last part of qualified name
    
    if is_optional:
        return f'Optional[{python_type}]'
    return python_type


def generate_pydantic_models(proto_data: Dict, output_file: str, title: str):
    """Generate Pydantic models from parsed protobuf data"""
    
    lines = [
        '"""',
        f'{title} - Auto-generated from protobuf definitions',
        '"""',
        '',
        'from __future__ import annotations',
        'from enum import Enum',
        'from typing import List, Optional, Dict, Any',
        'from pydantic import BaseModel, Field',
        '',
        ''
    ]
    
    # Generate enums
    for enum_name, enum_values in proto_data['enums'].items():
        lines.extend([
            f'class {enum_name}(str, Enum):',
            f'    """Auto-generated from protobuf enum {enum_name}"""'
        ])
        
        for value_name, value_num in enum_values.items():
            lines.append(f'    {value_name} = "{value_name}"')
        
        lines.extend(['', ''])
    
    # Generate message models
    for message_name, fields in proto_data['messages'].items():
        lines.extend([
            f'class {message_name}(BaseModel):',
            f'    """Auto-generated from protobuf message {message_name}"""'
        ])
        
        if not fields:
            lines.append('    pass')
        else:
            for field_name, field_info in fields.items():
                field_type = field_info['type']
                modifier = field_info['modifier']
                
                if modifier == 'repeated':
                    python_type = f'List[{proto_type_to_python(field_type)}]'
                    default = ' = Field(default_factory=list)'
                elif modifier == 'optional':
                    python_type = proto_type_to_python(field_type, is_optional=True)
                    default = ' = None'
                else:
                    python_type = proto_type_to_python(field_type)
                    default = ''
                
                lines.append(f'    {field_name}: {python_type}{default}')
        
        lines.extend(['', ''])
    
    # Add helper classes
    lines.extend([
        '# Helper classes for Connect/gRPC',
        'class EmptyResponse(BaseModel):',
        '    """Empty response for void operations"""',
        '    pass',
        '',
        '',
        'class ConnectError(BaseModel):',
        '    """Connect/gRPC error response"""',
        '    code: str',
        '    message: str',
        '    details: Optional[List[Dict[str, Any]]] = None',
        ''
    ])
    
    # Write to file
    with open(output_file, 'w') as f:
        f.write('\n'.join(lines))


def main():
    if len(sys.argv) != 4:
        print("Usage: generate_models.py <proto_dir> <output_file> <title>")
        sys.exit(1)
    
    proto_dir = sys.argv[1]
    output_file = sys.argv[2]
    title = sys.argv[3]
    
    print(f"🔄 Generating Pydantic models from {proto_dir}")
    
    # Find all .proto files
    proto_files = list(Path(proto_dir).glob('*.proto'))
    
    if not proto_files:
        print(f"❌ No .proto files found in {proto_dir}")
        sys.exit(1)
    
    # Parse all proto files
    all_data = {
        'enums': {},
        'messages': {},
        'services': {}
    }
    
    for proto_file in proto_files:
        print(f"📝 Parsing {proto_file.name}")
        try:
            data = parse_proto_file(str(proto_file))
            all_data['enums'].update(data['enums'])
            all_data['messages'].update(data['messages'])
            all_data['services'].update(data['services'])
        except Exception as e:
            print(f"⚠️  Warning: Could not parse {proto_file}: {e}")
    
    # Generate Pydantic models
    os.makedirs(os.path.dirname(output_file), exist_ok=True)
    generate_pydantic_models(all_data, output_file, title)
    
    print(f"✅ Generated {output_file}")
    print(f"📊 Found: {len(all_data['enums'])} enums, {len(all_data['messages'])} messages, {len(all_data['services'])} services")


if __name__ == '__main__':
    main()

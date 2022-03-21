import argparse
import re

parser = argparse.ArgumentParser()
parser.add_argument('input', type=str, help='Main GLSL file')
parser.add_argument('output', type=str, help='The output file to write')
parser.add_argument('--includepath', type=str, help='Include directory for the shaders', required=True)
args = parser.parse_args()

prog = re.compile('^#include "(.+)"$')

def replaceInclude(current_dir, filename):
    lines = []
    with open(f'{current_dir}/{filename}', 'r') as f:
        for line in f:
            re_result = prog.match(line)
            if re_result is not None:
                tokens = re_result.group(1).rsplit('/')
                if len(tokens) == 2:
                    path = tokens[0]
                    name = tokens[1]
                else:
                    path = ''
                    name = tokens[0]
                lines += replaceInclude(f'{current_dir}/{path}', name)
            else:
                lines.append(line)

    return lines

with open(args.output, 'w') as f:
    for line in replaceInclude(args.includepath, args.input):
        f.write(line)

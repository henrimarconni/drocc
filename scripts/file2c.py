import sys

with open(sys.argv[1], 'rb') as f:
  data = f.read()

with open(sys.argv[2], 'w') as out:
  out.write(', '.join(f"0x{b:02x}" for b in data))

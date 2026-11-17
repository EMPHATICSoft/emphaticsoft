#!/usr/bin/env python3
"""Write the difference (first - second) of two SSD alignment files.

Each file has lines "station plane sensor dx dy dz phi theta psi"
(translations in mm, ROOT Euler angles in degrees). Empty lines and lines
starting with '#' are ignored. The output has one line per sensor found in
either file, ordered by station, plane, sensor. A sensor missing from one file
is treated as having all-zero constants there. Angle differences are wrapped
into (-180, 180] degrees.

Example:
  diff_align.py new_constants.txt reference_constants.txt -o difference.txt
"""
import argparse
import sys


def wrap(angle):
    angle = (angle + 180.0) % 360.0 - 180.0
    return 180.0 if angle == -180.0 else angle


def read_align(path):
    constants = {}
    with open(path) as f:
        for number, line in enumerate(f, 1):
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            tok = line.split()
            if len(tok) < 9:
                sys.exit('%s:%d: expected 9 columns, found %d' % (path, number, len(tok)))
            try:
                key = tuple(int(t) for t in tok[:3])
                values = [float(t) for t in tok[3:9]]
            except ValueError:
                sys.exit('%s:%d: cannot parse line: %s' % (path, number, line))
            if key in constants:
                sys.exit('%s:%d: duplicate entry for station %d plane %d sensor %d'
                         % ((path, number) + key))
            constants[key] = values
    return constants


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('first', help='alignment file to subtract from')
    ap.add_argument('second', help='alignment file that is subtracted')
    ap.add_argument('-o', '--output', default='-',
                    help='output file (default: standard output)')
    ap.add_argument('--precision', type=int, default=8,
                    help='significant digits of the output values (default 8)')
    args = ap.parse_args()

    first = read_align(args.first)
    second = read_align(args.second)
    zero = [0.0] * 6

    out = sys.stdout if args.output == '-' else open(args.output, 'w')
    try:
        out.write('# %s - %s\n' % (args.first, args.second))
        out.write('# station plane sensor dx dy dz phi theta psi\n')
        for key in sorted(set(first) | set(second)):
            a = first.get(key, zero)
            b = second.get(key, zero)
            diff = [a[i] - b[i] for i in range(3)]
            diff += [wrap(a[i] - b[i]) for i in range(3, 6)]
            out.write('%d %d %d %s\n' % (key + (
                ' '.join('%.*g' % (args.precision, v) for v in diff),)))
    finally:
        if out is not sys.stdout:
            out.close()

    only_first = sorted(set(first) - set(second))
    only_second = sorted(set(second) - set(first))
    if only_first:
        print('Note: only in %s (second taken as zero): %s' % (args.first, only_first),
              file=sys.stderr)
    if only_second:
        print('Note: only in %s (first taken as zero): %s' % (args.second, only_second),
              file=sys.stderr)
    return 0


if __name__ == '__main__':
    sys.exit(main())

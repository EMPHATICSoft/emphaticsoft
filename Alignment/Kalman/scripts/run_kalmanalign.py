#!/usr/bin/env python3
"""Iterate KalmanAlign art jobs until the alignment constants converge.

Each iteration runs art with a generated fcl that includes the user's job fcl
plus overrides: the constants file from the previous iteration
(RunHistoryService AlignFilename) and the accumulated field-map offset. The
module writes a constants file (new transform = current + fitted correction)
and a text summary (fitted correction, sigma, chi2/innovation). Converged
when max |correction|/sigma < --epsilon over all free parameters.

Example:
  run_kalmanalign.py -c kalmanalign_job.fcl -s files.txt -w work \
      --max-iter 10 --epsilon 0.1 [--align-file start_constants.txt] \
      [-e 'physics.analyzers.kalmanalign.FixedParameters: ["1:0:*:*"]']
"""
import argparse
import os
import subprocess
import sys


def read_summary(path):
    header, params = {}, []
    with open(path) as f:
        for line in f:
            if line.startswith('##'):
                continue
            tok = line.split()
            if not tok:
                continue
            if tok[0] == '#':
                header[tok[1]] = float(tok[2])
            else:
                params.append((tok[0], float(tok[1]), float(tok[2])))
    return header, params


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('-c', '--config', required=True,
                    help='art fcl file: a path, or a name found via FHICL_FILE_PATH')
    ap.add_argument('-s', '--source', required=True,
                    help='input ROOT file, or a text file list (art -S)')
    ap.add_argument('-w', '--workdir', default='kalmanalign_work')
    ap.add_argument('-n', '--nevents', type=int, default=None)
    ap.add_argument('--max-iter', type=int, default=10)
    ap.add_argument('--epsilon', type=float, default=0.1,
                    help='converged when |dalpha| < epsilon * sigma')
    ap.add_argument('--align-file', default='',
                    help='starting constants file (default: the one chosen by RunHistory)')
    ap.add_argument('-e', '--extra', action='append', default=[],
                    help='extra fcl override line, appended to every job fcl')
    ap.add_argument('--module', default='kalmanalign',
                    help='label of the analyzer in the fcl')
    args = ap.parse_args()

    os.makedirs(args.workdir, exist_ok=True)
    # A config that exists as a file is used by absolute path; otherwise it is
    # a name to be found through FHICL_FILE_PATH, as art itself does.
    config = (os.path.abspath(args.config) if os.path.isfile(args.config)
              else args.config)
    align_file = os.path.abspath(args.align_file) if args.align_file else ''
    field = [0.0, 0.0]
    prefix = 'physics.analyzers.%s.' % args.module

    for it in range(1, args.max_iter + 1):
        d = os.path.abspath(os.path.join(args.workdir, 'iter%02d' % it))
        os.makedirs(d, exist_ok=True)
        constants = os.path.join(d, 'constants.txt')
        summary = os.path.join(d, 'summary.txt')
        fcl = os.path.join(d, 'job.fcl')
        with open(fcl, 'w') as f:
            f.write('#include "%s"\n' % config)
            f.write('%sOutputAlignFile: "%s"\n' % (prefix, constants))
            f.write('%sOutputSummaryFile: "%s"\n' % (prefix, summary))
            f.write('%sFieldOffsetX: %.10g\n' % (prefix, field[0]))
            f.write('%sFieldOffsetY: %.10g\n' % (prefix, field[1]))
            if align_file:
                f.write('services.RunHistoryService.AlignFilename: "%s"\n' % align_file)
            for line in args.extra:
                f.write(line + '\n')

        src = os.path.abspath(args.source)
        cmd = ['art', '-c', fcl]
        cmd += ['-S', src] if args.source.endswith(('.txt', '.list')) else [src]
        if args.nevents is not None:
            cmd += ['-n', str(args.nevents)]
        print('Iteration %d: %s' % (it, ' '.join(cmd)), flush=True)
        with open(os.path.join(d, 'art.log'), 'w') as log:
            rc = subprocess.call(cmd, cwd=d, stdout=log, stderr=subprocess.STDOUT)
        if rc != 0 or not os.path.exists(summary):
            print('art failed (rc=%d); see %s/art.log' % (rc, d), file=sys.stderr)
            return 1

        header, params = read_summary(summary)
        worst, worst_label = 0.0, ''
        for label, value, sigma in params:
            pull = abs(value) / sigma if sigma > 0 else 0.0
            if pull > worst:
                worst, worst_label = pull, label
            if label == 'magnet_FieldX':
                field[0] += value
            elif label == 'magnet_FieldY':
                field[1] += value
        print('  tracks=%d  chi2/innov=%.4f  max|dalpha|/sigma=%.3f (%s)  field offset=(%.4f, %.4f)'
              % (header.get('tracks', 0), header.get('chi2_per_innovation', 0.0),
                 worst, worst_label, field[0], field[1]), flush=True)

        # The next iteration starts from the constants written by this one.
        align_file = constants
        if worst < args.epsilon:
            print('Converged after %d iterations.' % it)
            break
    else:
        print('Not converged after %d iterations.' % args.max_iter)

    print('Final constants: %s' % align_file)
    print('Final field offset (mm): x=%.5f y=%.5f' % tuple(field))
    print('Covariance of the last iteration: tree "KalmanAlign" in its *_hist.root file')
    return 0


if __name__ == '__main__':
    sys.exit(main())

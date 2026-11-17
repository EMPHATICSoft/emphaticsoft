# Kalman SSD alignment

This directory contains the Kalman-based SSD alignment module, its parameter
and covariance code, configuration, tests, and iteration driver. It extends
the track propagation and filtering implementation in `KalmanReco`; the
alignment module itself is registered as `KalmanAlign`.

## Technique

The method is a sequential track-based alignment fit (B2):

1. For each selected track, `KTracker` performs a forward Kalman fit to the
   line-segment measurements. The module uses a refined fit to establish a
   linearization point, then makes a final pass with a diffuse track-state
   prior. An optional beam-momentum prior supplies external information on
   q/p.
2. Each scalar forward-filter innovation and its variance are treated as an
   alignment observation. Derivatives of the innovations with respect to the
   alignment parameters are propagated through the track fit.
3. `KAlignFilter` updates the alignment parameter means and the **full
   covariance matrix** recursively using the innovation observations.

The fitted sensor parameters are Tn (in-plane translation normal to the
strips), Tpar (translation along the strips), Tz (translation along the beam),
and Rz (rotation about the sensor center). Tilts are not fitted. The field-map
x and y translations can also be fitted. Tpar has no hit-position derivative;
without a strip-edge/acceptance observation it is prior-dominated, so it is
fixed by default.

The reported alignment parameters are corrections to the geometry currently
in use, including any constants loaded by `AlignService`. The output constants
file composes those corrections with the current constants. The output ROOT
tree contains the fitted parameter covariance and the covariance propagated
to the written sensor constants. The constants text file is ordered by
station, plane, then sensor; the text summary lists fitted parameters in the
same sensor order, with DOFs in parameter order and field parameters last.
The field translation is reported separately and must be supplied to the next
iteration.

## Build and tests

From the configured EMPHATICSoft build directory:

```sh
ninja KalmanReco KAlignFilter KalmanAlign \
  test_alignment_linear_algebra test_KAlignFilter test_KAlignParameters \
  test_KAlignDerivatives test_KAlignConstants test_ktracker_sensitivity
ctest -R 'test_alignment_linear_algebra|test_KAlign|test_ktracker_sensitivity' \
  --output-on-failure
```

`test_alignment_linear_algebra` checks the toy sequential-vs-batch linear
alignment calculation. The `test_KAlign*` tests cover the alignment filter,
parameter bookkeeping, derivatives, and constants/covariance composition.
`test_ktracker_sensitivity` checks the track sensitivity propagation,
including the magnetic-field translation.

## Configure a job

Use [kalmanalign_job.fcl](./kalmanalign_job.fcl) as a starting point. It
schedules the required data-quality, spill, SSD-cluster, and track-segment
reconstruction modules before `KalmanAlign`. Check that:

- The line-segment and track-segment labels in
  [KalmanAlign.fcl](./KalmanAlign.fcl) match the producer labels in the job.
- `AlignService` is enabled when the job should load alignment constants.
  The example job explicitly sets `services.AlignService.IsDisabled: false`.
- `services.RunHistoryService.AlignFilename` names the initial constants file
  when overriding the run-history default. Do not disable `AlignService` for
  an iterative alignment run: each iteration must use the previous iteration's
  constants.
- The `FixedParameters` list is appropriate for the geometry and its station,
  plane, and sensor numbering. The example fixes the agreed gauge parameters
  and all Tpar values; review that list before using another geometry.
- `BeamMomentum` and `BeamMomentumSigma` describe a valid external beam
  momentum prior for the tracks being selected. The example values are
  120 GeV/c and 2 GeV/c.

The job needs the raw SSD data products expected by the reconstruction
producers. If the input contains only pre-made line segments, remove or adapt
the reconstruction stages and ensure both labels consumed by `KalmanAlign`
are present.

## Run one job

After setting up the EMPHATICSoft runtime and building the module, run a small
sample first:

```sh
art -c Alignment/Kalman/kalmanalign_job.fcl -n 5000 input.root
```

Review the module's track/innovation counts, chi-square per innovation, and
the printed parameter values and uncertainties. Check the `KalmanAlign` tree
in the TFileService ROOT output for parameter labels, fitted values, sigma,
covariance, correlations, and the propagated constants covariance. The job
also writes `kalmanalign_constants.txt` and `kalmanalign_summary.txt` by
default; set `OutputAlignFile` or `OutputSummaryFile` to an empty string to
disable either output.

## Iterate to convergence

Once a single job is sensible, use the driver with the same input sample for
each iteration:

```sh
python3 Alignment/Kalman/scripts/run_kalmanalign.py \
  -c Alignment/Kalman/kalmanalign_job.fcl \
  -s input_files.txt \
  -w kalmanalign_work \
  --max-iter 10 \
  --epsilon 0.1 \
  -n 20000
```

Pass one ROOT file as `-s input.root`, or a text file containing ROOT file
paths as `-s input_files.txt` (the driver then uses `art -S`). An initial
constants file may be supplied with `--align-file`; otherwise the job's
RunHistory configuration chooses it. Use repeatable `-e 'FHiCL: override'`
arguments to override job settings, for example to change the fixed-parameter
list for a controlled test.

Each iteration writes its own `art.log`, FHiCL, constants file, summary, and
ROOT output under `kalmanalign_work/iterNN/`. The driver feeds the output
constants file into the next job and accumulates the fitted field-map
translations. It stops when the largest `|correction|/sigma` is below
`--epsilon`, or when `--max-iter` is reached. It prints chi-square per
innovation for monitoring but does not currently use a chi-square-change
criterion. Inspect the final iteration's covariance and verify that the
alignment constants and field offsets are physically reasonable before
adopting them.

## Plot the covariance and correlation

The `KalmanAlign` tree in the `*_hist.root` file holds the covariance and
correlation matrices of the free parameters. The macro draws three 2D
histograms with the parameter labels on the axes: the covariance, the signed
square root `sign(C_ij)*sqrt(|C_ij|)` (diagonal = sigma, so small and large
entries are comparable; units are mm or rad), and the correlation:

```sh
root -l -b -q 'Alignment/Kalman/scripts/plot_covariance.C("kalmanalign_work/iter05/<name>_kalmanalign_hist.root", "cov_corr.pdf")'
```

Omit the second argument to just display the canvas. A third argument
(for example `"cov_corr.root"`) also writes the histograms `hCovariance`,
`hSignedSqrt` and `hCorrelation` and the canvas `cCov` to a ROOT file; the tree
name is an optional fourth argument.

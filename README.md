# Scrappie basecaller

Scrappie is a technology demonstrator for the Oxford Nanopore Research Algorithms group.
```
Ref   : GACACAGTGAGGCTGCGTCTC-AAAAAAAAAAAAAAAAAAAAAAAAATTGCCCCTTCTTAAGTTTGCATTTAGATCTCTT
Query : GACACAG-GAGGCTGCGTCTCAAAAAAAAAAAAAAAAAAAAAAAAAATTGCCCCTTCTTAAGCTT-CA--CAGA-CT-TT
```

For a complete release history, see [RELEASES.md]

## Dependencies
* A good BLAS library + development headers including cblas.h.
* The HDF5 library and development headers.

On Debian based systems, the following packages are sufficient (tested Ubuntu 14.04 and 16.04)
* libopenblas-base
* libopenblas-dev
* libhdf5
* libhdf5-dev

## Compiling
```bash
mkdir build && cd build && cmake .. && make
```

## Running
```bash
#  Set some enviromental variables.  
# Allow scrappie to use as many threads as the system will support
export OMP_NUM_THREADS=`nproc`
# Use openblas in single-threaded mode
export OPENBLAS_NUM_THREADS=1
# Reads can be specified as individual files
scrappie reads/read1.fast5 reads/read2.fast5 ... > basecalls.fa
# or entire folders
scrappie reads > basecalls.fa
# Or using a strand list (skipping first line)
tail -n +2 strand_list.txt | sed 's:^:/path/to/reads/:' | xargs scrappie > basecalls.fa
```

## Commandline options
```
> scrappie --help
Usage: scrappie [OPTION...] fast5 [fast5 ...]
Scrappie basecaller -- scrappie attempts to call homopolymers

  -#, --threads=nreads       Number of reads to call in parallel
  -a, --analysis=number      Analysis to read events from
      --albacore, --no-albacore   Assume fast5 have been called using Albacore
      --dump=filename        Dump annotated events to HDF5 file
      --dwell, --no-dwell    Perform dwell correction of homopolymer lengths
      --hdf5-chunk=size      Chunk size for HDF5 output
      --hdf5-compression=level   Gzip compression level for HDF5 output (0:off,
                             1: quickest, 9: best)
  -l, --limit=nreads         Maximum number of reads to call (0 is unlimited)
      --licence, --license   Print licensing information
  -m, --min_prob=probability Minimum bound on probability of match
  -o, --outformat=format     Format to output reads (FASTA or SAM)
  -s, --skip=penalty         Penalty for skipping a base
      --segmentation=group   Fast5 group from which to reads segmentation
      --segmentation-analysis=number
                             Analysis number to read seqmentation fro
      --slip, --no-slip      Use slipping
  -t, --trim=nevents         Number of events to trim
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
```


```
> scrappie_raw --help
Usage: scrappie_raw [OPTION...] fast5 [fast5 ...]
Scrappie basecaller -- basecall from raw signal

  -#, --threads=nreads       Number of reads to call in parallel
      --dump=filename        Dump annotated blocks to HDF5 file
      --hdf5-chunk=size      Chunk size for HDF5 output
      --hdf5-compression=level   Gzip compression level for HDF5 output (0:off,
                             1: quickest, 9: best)
  -l, --limit=nreads         Maximum number of reads to call (0 is unlimited)
      --licence, --license   Print licensing information
  -m, --min_prob=probability Minimum bound on probability of match
  -o, --outformat=format     Format to output reads (FASTA or SAM)
  -s, --skip=penalty         Penalty for skipping a base
      --slip, --no-slip      Use slipping
  -t, --trim=nsamples        Number of samples to trim from either end
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
```

## Output formats
Scrappie current supports two ouput formats, FASTA and SAM.  The default format is currently FASTA;
SAM format output is enabled using the `--outformat SAM` commandline argument.

Scrappie can emit SAM "alignment" lines containing the sequences but no quality information.  No other fields, include a SAM header are emitted.  A BAM file can be obtained using `samtools` (tested with version 0.1.19-96b5f2294a) as follows:

```bash
scrappie -o sam reads | samtools -Sb - > output.bam
```

### FASTA
When the output is set to FASTA (default) then some metadata is stored in the description
  * The sequence ID is the name of the file that was basecalled.
  * The *description* element of the FASTA header is a JSON string containing the following elements:
    * `normalised_score` Normalised score (total score / number of events or blocks).
    * `nevents` Number of events
    * `sequence_length` Length of sequence called
    * `events_per_base` Number of events per base called



## Gotya's and notes
* Scrappie does not call events and relies on this information already being present in the fast5 files.  In particular:
  * Event calls are taken from (where `XXX` is the number set by the `--analysis` flag)
    * `--no-albacore` (default) --> `/Analyses/EventDetection_XXX/Reads/Read_???/Events`
    * `--albacore` --> `/Analyses/Basecall_1D_XXX/BaseCalled_template/Events`
  * Segmentation is taken (by default) from /Analyses/Segment\_Linear\_XXX/Summary/split\_hairpin.  The group name for the segmentation data, here Segment\_Linear, can be set using the `--segmentation` flag.
* Model is hard-coded.  Generate new header files using `parse_lstm.py model.pkl > lstm_model.h`
* The normalised score (- total score / number of events) correlates well with read accuracy.
* Events with unusual rate metrics (number of events or blocks / bases called) may be unreliable.

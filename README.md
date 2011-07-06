Installation
============

    sudo gem install sam

Description
===========

This gem provides the `Sam` class which has the following member variables:

* `name`: a string
* `flag`: an integer or a string if `samtools view -X` was used
* `chrom`: a string
* `pos`: an integer (-1 if unmapped)
* `mapq`: an integer
* `cigar`: a string
* `mchrom`: a string
* `mpos`: an integer (-1 if unmapped)
* `insert`: an integer (or the raw string if not set)
* `seq`: a string
* `qual`: a string
* `tags`: a hash mapping tag names to tag values

Usage
=====

To print the XT:U tag of each entry, you might do this:
    require 'sam'

    for line in gets
      next if line[0] == "@" # Skip header if it exists
      puts Sam.new(line).tags["XT:U"]
    end

You can do something similar with a one-liner at the command line

    cat test.sam | ruby -r sam -ne 'next if $_[0] == "@"; puts Sam.new($_).tags["XT:U"]' 

You can also use the `parse_line` method to reuse the same Sam object

    require 'sam'
    s = Sam.new

    for line in gets
      if s.parse_line(line) # Returns false if line starts with @ (a header line)
        puts s.tags["XT:U"]
      end
    end

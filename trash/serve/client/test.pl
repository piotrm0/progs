#!/usr/local/bin/perl

use IPC::Open2;
use IO::Select;
use IO::Handle;
use Fcntl;

test();

sub test {
  local *out;

  sysopen(out, "test.out", O_WRONLY);

  print out "lolol\n";

  close(out);
}


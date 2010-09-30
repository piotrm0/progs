#!/usr/local/bin/perl

# pm, june 08, 2008

$DIR = '/flex_sdk_3/bin';

$MKFIFO = 'mkfifo';

use IPC::Open3;
use IO::Select;
use IO::Handle;
use Fcntl;

$SELF = "async_fcsh";

$ID = `id -u`;
chomp $ID;

$PIPE  = "/tmp/async_fcsh.$ID.fifo";
$PIPE2 = "/tmp/async_fcsh.$ID.fifo2";

$cmd = join(" ", @ARGV);

if ($cmd eq "clean") {
  cleanup();
  print "$SELF(m): cleaned up\n";
  exit(0);
}

check_pipe($PIPE);
check_pipe($PIPE2);

run_or_start(0);

sub run_or_start {
  my ($attempt) = @_;

  #print "$SELF(m): (attempt $attempt) opening\n";

  local *out;
  local *in;

  if (0 >= sysopen(out, $PIPE2, O_WRONLY | O_NONBLOCK)) {
    if (0 != $attempt) {
      print "$SELF(m): !!! (attempt $attempt) open out failed\n";
      print "$SELF(m): !!! (attempt $attempt) errno = " . $! . "\n";
    }

    if ($attempt == 1) {
      print stderr "$SELF(m): !!! (attempt $attempt) cannot start\n";
      exit(1);
    } else {
      if (0 == fork) {
	close(out);
	return start();
      } else {
	return run_or_start(1);
      }
    }
  }

  #print "$SELF(m): (attempt $attempt) done\n";

  syswrite(out, $cmd . "\n");

  close(out);

  if (0 >= sysopen(in,  $PIPE, O_RDONLY)) {
    print "$SELF(m): !!! (attempt $attempt) open in failed\n";
    exit(1);
  }

  #print "$SELF(m): wrote $cmd\n";

  my $temp = "";
  while(sysread(in, $temp, 1024)) {
    #print "$SELF(m): got $temp";
    print $temp;
  }

  #print "$SELF(m): done reading\n";

  close(in);
}

sub start {
  my $fcsh = "$DIR/fcsh";
  #print "$SELF(l): starting fcsh (using $fcsh)\n";

  my $cmd_line = "";  # stores command as it is being read in
  my $cmd_mem  = {};  # command -> target id association
  my $cmd_last = "";  # the last command sent to fcsh
  my $buff      = ""; # buffer as things are being read
  my $buff_last = ""; # buffer stores the entire output, not just whatever hasn't been sent back yet

  my $first     = 1;  # has fcsh just been started ?
  my $had_error = 0;  # did fcsh print anything to stderr ?

  local *pipe_in ; local *pipe_out;
  local *prog_in ; local *prog_out; local *prog_err;

  #print "$SELF(l): prog_* = " . \*prog_in . \*prog_out . \*prog_err . "\n";
  #print "$SELF(l): pipe_* = " . \*pipe_in . \*pipe_out . "\n";

  my $pipe_out_open = 0;

  sub open_pipe {
    my ($pipe, $filename, $flags) = @_;
    if (0 >= sysopen($pipe, $filename, $flags)) {
      print "$SELF(l): !!! could not open pipe $filename\n";
      print "$SELF(l): !!! errno = $!\n";
      return 1;
    } else {
      #print "$SELF(l): opened pipe_in\n";
      return 0;
    }
  };
  sub open_pipe_in {
    return open_pipe(pipe_in, $PIPE2, O_RDONLY | O_NONBLOCK);
  };
  sub open_pipe_out {
    return 0 if ($pipe_out_open);
    if (! open_pipe(pipe_out, $PIPE, O_WRONLY)) {
      $pipe_out_open = 1;
      return 0;
    } else {
      return 1;
    }
  };
  sub write_to_out {
    my ($line) = @_;
    syswrite(pipe_out, $line) if ($pipe_out_open);
  }

  sub add_to_buffer {
    my ($temp) = @_;
    $buff      .= $temp;
    $buff_last .= $temp;

    @lines = split(/\n/, $buff, -1);
    $buff = pop(@lines);

    return @lines;
  };

  sub process_in {
    my ($pipe) = @_;
    my $temp;
    if (0 == sysread($pipe, $temp, 1024)) {
      #print stderr "$SELF(l): !!! $pipe closed\n";
      write_to_out("$SELF(l): fcsh quit, closing listener\n");
      cleanup(); exit(0);
    }

    $had_error = 1 if $pipe == \*prog_err;

    @lines = add_to_buffer($temp);

    for $line (@lines) {
      #print "$SELF(l): read from $pipe: $line\n";
      write_to_out($line . "\n");
    }
  }

  my $handlers = {};

  $handlers->{\*pipe_in} = sub {
    my $line = <pipe_in>;

    if (not $line) {
      #print stderr "$SELF(l): !!! pipe_in closed\n";

      close(pipe_in); open_pipe_in();
      open_pipe_out();

      my $cmd_actual = $cmd_line;

      if ($cmd_mem->{$cmd_line}) {
	$cmd_actual = "compile " . $cmd_mem->{$cmd_line} . "\n";
      }

      print "$SELF(l): running: $cmd_actual";

      $had_error = 0;

      syswrite(prog_out, $cmd_actual);

      $cmd_last = $cmd_line;
      $cmd_line = "";

    } else {
      #print "$SELF(l): read from pipe_in: $line";
      $cmd_line .= $line;

    }
  };

  $handlers->{\*prog_err} = sub {
    process_in(\*prog_err);
  };

  $handlers->{\*prog_in} = sub {
    process_in(\*prog_in);

    if ($buff eq "(fcsh) ") {
      $buff = "";

      if ($buff_last =~ m/fcsh: Assigned ([0-9]+) as the compile target id/) {
	my $id = $1;
	#print "$SELF(l): had_error = $had_error\n";
	if (($had_error) and ($buff_last =~ m/Error: unable to open /)) {
	  print "$SELF(l): got id [$id] but it isn't going to be used\n";
	} else {
	  $cmd_mem->{$cmd_last} = $id;
	  print "$SELF(l): assigned id [$id] to [$cmd_last]\n";
	}
      }

      $buff_last = "";

      if (not $first) {
	close(pipe_out);
	$pipe_out_open = 0;
	$cmd_last  = "";

      } else {
	$first = 0;
      }
    }
  };

  open_pipe_in();

  my $pid = open3(\*prog_out, \*prog_in, \*prog_err, $fcsh);
  #print "$SELF(l): opened fcsh at $pid\n";

  my $s = IO::Select->new();
  $s->add(\*pipe_in);
  $s->add(\*prog_in);

  $s->add(\*prog_err);

  while(1) {
    my ($reads, $writes, $errors) = IO::Select->select($s, undef, undef);

    #print "$SELF(l): reads  = [" . join(",", @$reads)  . "]\n";
    #print "$SELF(l): writes = [" . join(",", @$writes) . "]\n";
    #print "$SELF(l): errors = [" . join(",", @$errors) . "]\n";

    foreach $r (@$reads) {
      #print "$SELF(l): handling $r\n";
      $handlers->{$r}();
    }
  }

  return;
}

sub check_pipe {
  my ($pipe) = @_;

  if ((-e $pipe) &&
      (not -p $pipe)) {
    print stderr "$SELF(m): $pipe exists but is not a pipe\n";
    exit(1);
  }
  if (not -e $pipe) {
    make_pipe($pipe);
  }

  return;
}

sub make_pipe {
  my ($pipe) = @_;

  #print "making pipe at $pipe\n";

  system($MKFIFO, $pipe);
  if ($? == -1) {
    print stderr "cannot run mkfifo (using $MKFIFO)\n";
    exit(1);
  } elsif ($? & 127) {
    print stderr "cannot make pipe\n";
    exit(1);
  } elsif ($? >> 8) {
    print stderr "cannot make pipe\n";
    exit(1);
  }

  if (not -p $pipe) {
    print stderr "could not make pipe\n";
    exit(1);
  }

  `chmod og-rwx $pipe`;

  return;
}

sub cleanup {
  unlink ($PIPE)  if -e $PIPE;
  unlink ($PIPE2) if -e $PIPE2;
}

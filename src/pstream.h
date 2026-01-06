#ifndef __BLOP_PSTREAM_H__
#define __BLOP_PSTREAM_H__

#include "var.h"
#include <iostream>
#include <fstream>
#include <signal.h>

namespace blop
{

// The classes 'ipstream' and 'opstream' (input- and output-pipe-streams)
// implement the important task of handling pipes (external programs, with
// in- or output redicted) as streams


class pipebuf;

class pstream_base
{
protected:
    pipebuf *buffer_;

public:
    void kill(int sig=SIGTERM);
    bool is_open() const;
    std::string cmd() const;
};

class ipstream : public std::istream, public pstream_base
{
 private:

    ipstream(const ipstream &);
    void operator= (const ipstream &) {}

 public:
    void ibufsize(int size);

    ipstream(const var  &name);
    ipstream();
    ~ipstream();

    ipstream &open(const var &name);

    // =-=-=- closes the stream, and returns the exit status =-=-=-=
    // of the process, or -1 on error
    int close();

};

class opstream : public std::ostream, public pstream_base
{
 private:
    opstream(const opstream &);
    void operator= (const opstream &) {}

 public:
    void obufsize(int size);

    opstream(const var &name);
    opstream();
    ~opstream();

    opstream &open(const var &name);

    // =-=-=- closes the stream, and returns the exit status =-=-=-=
    // of the process, or -1 on error    
    int close();

};

// ---------------------------------------------------
class iremotestream : public ifstream
{
 protected:
    std::string local_filename_, remote_filename_, cmd_template_;
 public:
    iremotestream (const string &cmd) : cmd_template_(cmd) {}
    iremotestream (const string &cmd, const char *, ios_base::openmode mode = ios_base::in );
    void open(const char *,ios_base::openmode mode = ios_base::in );
};

class iscpstream : public iremotestream
{
 public:
    iscpstream() : iremotestream("scp %s %s") {}
    iscpstream(const char *name, ios_base::openmode mode = ios_base::in) ;
};

class ihttpstream : public iremotestream
{
 public:
    ihttpstream() : iremotestream("wget %s -O %s") {}
    ihttpstream(const char *name, ios_base::openmode modeo = ios_base::in);
};

class oremotestream : public ofstream
{
 protected:
    std::string local_filename_, remote_filename_, cmd_template_;
 public:
    oremotestream(const string &cmd) : cmd_template_(cmd) {}
    oremotestream(const string &cmd, const char *name, ios_base::openmode mode = ios_base::out);
    ~oremotestream();
    void open(const char *name, ios_base::openmode mode = ios_base::out);
    void close();
};

class oscpstream : public oremotestream
{
 public:
    oscpstream() : oremotestream("scp %s %s") {}
    oscpstream(const char *name, ios_base::openmode mode = ios_base::out);
};

}


#endif

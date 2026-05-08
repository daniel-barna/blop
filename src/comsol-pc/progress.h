#ifndef __BLOP_PROGRESS_H__
#define __BLOP_PROGRESS_H__

/*

  A utility class to display progress of a long process on the terminal. Usage:

  const int N = 10000;
  progress prog("Looping to " + std::to_string(N) + "... ",2);   // Print the text to the screen, and then show status in steps of 2 (percent)
  for(int i=0; i<N; ++i)
  {
     prog(double(i)/N*100);      // The default 'unit' to be written after the value is %, so we multiply by 100
     prog(double(i)/N*100,"%");  // Same as the previous line, the default is written explicitely
     prog(i," (current value)"); // report the current value simply, without trailing '%'
  }

  
*/

namespace blop
{

class progress
{
private:
    double last_value_ = 0;
    double step_ = 1;
    std::string before_;
    int last_width_ = -1;  // -1 indicates we have not written anything yet, >=0 means we have written something (possibly also 0-length, if last_width_==0)


    // Was meant to handle utf-8 characters, for example checkmark ✅, but it doesn't work, 
    size_t utf8_length(const std::string& s)
        {
            size_t count = 0;
            for (unsigned char c : s)
                if ((c & 0xC0) != 0x80)  // not a continuation byte
                    ++count;
            return count;
        }    

public:

    progress(const std::string &before="", double step=1) : step_(step), before_(before)
        {
            std::cerr<<before_;
        }
    void operator()(double value, const std::string &after = "%")
        {
            if(value >= last_value_ + step_)
            {
                if(last_width_ >= 0) for(int i=0; i<last_width_; ++i) std::cerr<<"\b";
                std::ostringstream s;
                s<<value<<after<<" ";
                std::cerr<<s.str();
                last_value_ = value;
                last_width_ = s.str().size();
                //last_width_ = utf8_length(s.str());
            }
        }
};

}

#endif

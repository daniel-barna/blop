#ifndef __BLOP_PRINTER_H__
#define __BLOP_PRINTER_H__

#include "eps.h"
#include "var.h"


namespace blop
{

    class printer : public eps
    {
    private:
	typedef eps base;

	std::string cmd_;
	static std::string default_cmd_;

    public:
	static var        default_command();
	static void       default_command(const var &);

	static void    print(const var &cmd = printer::default_command(), const var &beps2epsopts = "");
	printer(const var &cmd = printer::default_command(), const var &epsopts = "");
	~printer();
	void picture_end();
    };

}



#endif

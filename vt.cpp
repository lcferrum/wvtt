#include "labeled_values.hpp"
#include <iostream>
#include <iomanip>
#include <limits>
#include "externs.h"

#define COUT_DEC(dec_int)				+dec_int
#define COUT_HEX(hex_int, hex_width)	"0x"<<std::hex<<std::setw(hex_width)<<+hex_int<<std::dec
#define COUT_BOOL(bool_val)				(bool_val?"TRUE":"FALSE")

int main(int argc, char* argv[])
{
	//This sets fill character to '0' for all subsequent cout outputs 
	//It only matters when setw in non-zero (and it is reset to zero after every output) so it won't corrupt ordinary integer output
	std::cout.fill('0');

	//This prevents base from showing in hex/oct output and hex/oct output becomes uppercase
	//N.B.: Uppercase also makes base to be displayed in uppercase (that's why showing base is disabled) but doesn't affect boolalpha flag (bool values still displayed in lowercase)
	//Applied for all subsequent cout outputs 
	std::cout.unsetf(std::ios::showbase);
	std::cout.setf(std::ios::uppercase);  
	
	LabeledValues SuiteMasks(LABELED_VALUES_ARG(VER_WORKSTATION_NT, VER_SERVER_NT, VER_SUITE_SMALLBUSINESS, VER_SUITE_ENTERPRISE, VER_SUITE_BACKOFFICE, VER_SUITE_COMMUNICATIONS, VER_SUITE_TERMINAL, VER_SUITE_SMALLBUSINESS_RESTRICTED, VER_SUITE_EMBEDDEDNT, VER_SUITE_DATACENTER, VER_SUITE_SINGLEUSERTS, VER_SUITE_PERSONAL, VER_SUITE_BLADE, VER_SUITE_EMBEDDED_RESTRICTED, VER_SUITE_SECURITY_APPLIANCE, VER_SUITE_STORAGE_SERVER, VER_SUITE_COMPUTE_SERVER, VER_SUITE_WH_SERVER));
    
	DWORD test1=VER_WORKSTATION_NT;
	DWORD test2=VER_SUITE_SMALLBUSINESS|VER_SUITE_BACKOFFICE;
	DWORD test3=VER_SUITE_SMALLBUSINESS|VER_SUITE_BACKOFFICE|0x100000;
	DWORD test4=777;
	
	std::cout<<"TEST3:\n"<<SuiteMasks.Flags(test3, [](const std::string& label, DWORD value, bool unknown, bool first){
		if (unknown)
			return std::string("\tUNKNOWN FLAG (")+std::to_string(value)+")\n";
		else
			return std::string("\t")+label+"\n";
	});
	
	std::cout<<"TEST2: "<<SuiteMasks.Flags(test2)<<std::endl;
	std::cout<<"TEST1: "<<SuiteMasks(test1)<<std::endl;
	std::cout<<"ALL: "<<SuiteMasks.Values()<<std::endl;
	std::cout<<"Find VER_SUITE_COMMUNICATIONS: "<<COUT_HEX(SuiteMasks.Find("VER_SUITE_COMMUNICATIONS", 0x0), 8)<<std::endl;
	
	std::cout<<"Press ENTER to continue..."<<std::flush;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	//Needs defined NOMINMAX
	return 0;
}
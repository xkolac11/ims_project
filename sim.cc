#include "sim.hh"

FILE *out = stdout;
bool out_to_close = false;

using namespace std;

void SetOut(const char *fd){

    if(fd != NULL){
        out = fopen(fd, "w+");
        if(out == NULL){
            //failed to open file, use stdout
            out = stdout;
        }else{
            //file opened successfuly, set the flag
            out_to_close = true;
        }
    }else{
       //stdout
       out = stdout;
    }

}

void Print(const char *str){
	fprintf(out, "%s", str);
}

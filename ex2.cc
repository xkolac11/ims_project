#include "sim.hh"

int main(){
    SetOut("hahaha");
    Print("neconeconeco\n");

    if(out_to_close == true){
        fclose(out);
        out_to_close = false;
    }

}

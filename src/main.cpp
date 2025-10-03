#include "c8vm.h"
#include "defs.h"

int main(int argc, char const *argv[])
{
  VM vm;
  vm.inicializar(0x200);
  vm.carregarROM(argv[1], 0x200);
  #ifdef DEBUG
    vm.imprimirRegistradores();
  #endif
  while(1) {
    vm.executarInstrucao();
    #ifdef DEBUG
      vm.imprimirRegistradores();
    #endif
  }
  return 0;
}

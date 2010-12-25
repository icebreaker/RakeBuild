#include <stdio.h>
#include "plugin2.h"
#include "../plugin1/plugin1.h"

void plugin2(void)
{
	printf("RakeBuild Hello From Plugin 2 Shared Object\n");
	plugin1();
}

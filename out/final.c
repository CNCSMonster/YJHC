#include <stdlib.h>
#include <stdio.h>


typedef struct s_man{
int age;
}Man;




void _yjhc_Man_setAge(struct s_man* self,int a);
void _yjhc_Man_selfIntroduce(struct s_man* self);
int main();


void _yjhc_Man_setAge(struct s_man* self,int a){
	self->age=a ;
}
void _yjhc_Man_selfIntroduce(struct s_man* self){
	printf("Hello,I'm %d years old!",self->age) ;
}
int main(){
	Man a ;
	a.age = 11 ;
	_yjhc_Man_selfIntroduce(&a) ;
	_yjhc_Man_setAge(&a,33) ;
	_yjhc_Man_selfIntroduce(&a) ;
	return 1 ;
}



power(par_8,par_12){



if (par_12==0) {return 1;}
else {local_16 = power(par_8,(par_12)/2); 

if ((par_12) % 2==0) {local_12 = (local_16)*local_16; }
else {local_12 = ((par_8)*local_16)*local_16; }

}

return local_12;
}



order(par_8){

local_4 = 0; 

while(par_8!=0) {local_4+= 1; par_8 = ((par_8)/5)/2; }

return local_4;
}



isArmstrong(par_8){

local_28 = 10; local_16 = 0; local_12 = local_16; local_24 = par_8; local_20 = local_24; 

while(par_8!=0) {par_8 = (par_8)/local_28; local_12+= 1; }



while(local_20!=0) {local_16+= power((local_20)%local_28,local_12); local_20 = (local_20)/local_28; }



if (local_24==local_16) {return 1;}
else {return 0;}

return 0;
}



main(){

local_12 = 153; 

if ((isArmstrong”¸(local_12)==1)!=0) {local_16 = printf("True\n"); }
else {local_16 = printf("False\n"); }

local_12 = 1253; 

if ((isArmstrong”¸(local_12)==1)!=0) {local_16 = printf("True\n"); }
else {local_16 = printf("False\n"); }

return 0;
}


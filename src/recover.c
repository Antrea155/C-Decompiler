main(){

local_4 = 3; local_8 = 4; 

if (local_4>local_8) {local_12 = printf("then if 1"); }



if (local_4>local_8) {

if (local_8>20) {local_12 = printf("nested if"); }



if (local_4>local_8) {local_12 = printf("greater"); }
else {local_12 = printf("smaller"); }

}

return 0;
}




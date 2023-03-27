main(){

local_12 = 3; local_16 = 4; 

if (local_12>local_16) {local_20 = printf("then if 1"); }



if (local_12>local_16) {

if (local_16>20) {local_20 = printf("nested if"); }



if (local_12>local_16) {local_20 = printf("greater"); }
else {local_20 = printf("smaller"); }

}

return 0;
}


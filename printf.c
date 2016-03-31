#include "printf.h"

/*********************************************************/
/*****************Basic Tool Functions********************/
/*********************************************************/
/*
 * Shift a chuck of chars for the front of array
 * Ex: "0000134" -> "1340000";
 */
void shiftCharsToFront(char *buf, int n, int start_i){
    //Basic error checking
    if (!buf || n <= 0) return;

    char buftmp[n];
    int i = 0;
    while (buf[start_i] != '\0'){
        buftmp[i] = buf[start_i];
        i++;
        start_i++;
    }
    buftmp[i] = '\0';

    for (int k = 0; k < n; k++){
        buf[k] = buftmp[k];
    }
}

void strCopy(char *buf, const char *target){
    while(( *buf++ = *target++ ));
}


int powOfNum(int num, int n){
    if (n <= 0) return 1;
    int res = 1;
    while (n > 0){
        res *= num;
        n --;
    }
    return res;
}
//Divide by 10 algorithm 
unsigned diviu10(unsigned n){
	unsigned q, r;
    q = (n >> 1) + (n >> 2);
    q = q + (q >> 4);
    q = q + (q >> 8);
    q = q + (q >> 16);
    q = q >> 3;
    r = n - (((q << 2) + q) << 1);
    return q + (r > 9);
}

int getStrLen(const char *str){
    const char* s;
    for (s = str; *s; ++s);
    return (s - str);
}

   


/*********************************************************/


char* inttobin( char *buf, int n, unsigned val, int width){
    //Add EOF
    buf[n-1] = '\0';
    int i = n - 2;

    //Special case
    if (width == 0 && val == 0){
        buf[i] = '0';
        shiftCharsToFront(buf, n, i);
        return buf; 
    }

    while (width > 0 && i >= 0){
        buf[i] = '0' + (val % 2);
        val = val >> 1;
        i --;
        width --;
    }


    //If val still greater than 0
    while (val > 0 && i >= 0){
        buf[i] = '0' + (val % 2);
        val = val >> 1;
        i --;
    }

    shiftCharsToFront(buf, n, i+1);
    return buf;
}

char* inttodec( char *buf, int n, int val, int width){
    //Add EOF
    buf[n-1] = '\0';
    int isNeg = FALSE;
    if (val < 0){
        isNeg = TRUE;
        val *= -1;
        width -= 1; //leave a space for "minus" sign
    }
    int i = n - 2;
    //Special case
    if (width == 0 && val == 0){
        buf[i] = '0';
        shiftCharsToFront(buf, n, i);
        return buf; 
    }


    while (width > 0 && i >= 0){
        buf[i] = '0' + (val % 10);
        val = (int)diviu10((unsigned)val);
        i --;
        width --;
    }
    //If val still greater than 0
    while (val > 0 && i >= 0){
        buf[i] = '0' + (val % 10);
        val = (int)diviu10((unsigned)val);
        i --;
    }
    
    //Check sign
    if (isNeg && i >= 0){
        buf[i] = '-';
        i --;
    }
    shiftCharsToFront(buf, n, i+1);
    return buf;
}

char* inttohex( char *buf, int n, unsigned val, int width){
    //Add EOF
    buf[n-1] = '\0';
    int i = n - 2;
    int rem = 0;
    //Special case
    if (width == 0 && val == 0){
        buf[i] = '0';
        shiftCharsToFront(buf, n, i);
        return buf; 
    }

    while (width > 0 && i >= 0){
        rem = val % 16;
        if (rem >= 10){
            rem -= 10;
            buf[i] = 'A' + rem;   
        }
        else{
            buf[i] = '0' + rem;
        }
        val = val >> 4;
        i --;
        width --;
    }
    //If val still greater than 0
    while (val > 0 && i >= 0){
        rem = val % 16;
        if (rem >= 10){
            rem -= 10;
            buf[i] = 'A' + rem;   
        }
        else{
            buf[i] = '0' + rem;
        }
        val = val >> 4;
        i --;
    }
    
    shiftCharsToFront(buf, n, i+1);
    return buf;
}
/*
 * transform numbers of array (char*) to int value
 * EX: "0031" -> 31
 *     "0204" -> 204
 */
int myAtoi(char* str){
    int res = 0; 
    int i = 0;
    int isNeg = 0;
    if (str[0] == '-'){
        i = 1;
        isNeg = 1;
    }
    for (; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';

    if (isNeg) res *= -1;
  
    /*int n = 0;*/
    /*while (chs[n] != '\0'){*/
        /*n++;        */
    /*}*/
    /*int res    = 0;*/
    /*for (int i = 0; i < n; i++){*/
        /*res += (chs[i] - '0') * powOfNum(10, n - i - 1);*/
    /*}*/
    return res;
}


/*
 * Compute the number of '%' signs followed by "c", "s", "d", "x", "b"
 * return -> number of chars
 */
int getOptions(char* input, char* options, int* widths){
    int ind = 0;
    int n = 0;
    while (input[ind] != '\0'){
        /*printf("%c\n", input[ind]);*/
        if (input[ind] == '%'){
            char width[MAXWIDTH];
            int i = 0;
            ind ++;
         
            while(input[ind] == '0' ||
                  input[ind] == '1' ||
                  input[ind] == '2' ||
                  input[ind] == '3' ||
                  input[ind] == '4' ||
                  input[ind] == '5' ||
                  input[ind] == '6' ||
                  input[ind] == '7' ||
                  input[ind] == '8' ||
                  input[ind] == '9'){

                width[i] = input[ind];
                i ++;
                ind ++;
            }
            //Append EOF to width
            width[i] = '\0';
            if (input[ind] == 'c' ||
                input[ind] == 's' ||
                input[ind] == 'd' ||
                input[ind] == 'x' ||
                input[ind] == 'b'){

                options[n] = input[ind];
                widths[n]  = myAtoi(width);
                n ++;
            }
            else if (input[ind] == '%'){ //FIXME -> what if '%%%%...' ??
                ind ++;
                continue;
            }
            else return -1; //If melformated...
        }
        //Move to next..
        ind ++;        
    }
    //The end of array
    options[n] = '\0';
    widths[n]  = '\0';

    return n;
}
/*
 * Replace '%..." with corresponding strings
 * @param -> bufs -> strings to plug in
 * @param -> m    -> size of bufs
 */
void replaceWithCorrChars(char *buf, int n, char *format, char bufs[MAXOPTIONS][MAXWIDTH]){
    //Error checking
    //if (!buf || !bufs) return;

    int start_i = 0;
    int start_j = 0;
    int start_k = 0;

    while (start_i < n-1 && format[start_j] != '\0'){
        if (format[start_j] == '%'){
            //Deal with multiple '%' signs
            if (format[start_j + 1] == '%'){
                buf[start_i] = '%';
                start_i ++;
                start_j += 2;
            }
            else{
                //Insert target 
                int ind = 0; //Ind of bufs[start_k]
                while (start_i < n-1 && bufs[start_k][ind] != '\0'){
                    buf[start_i] = bufs[start_k][ind];
                    start_i ++;
                    ind ++;
                }   
                //Jump to next token
                start_k ++;
                

                //Skip "%00..X"
                while (format[start_j] == '%' ||
                       format[start_j] == '0' ||
                       format[start_j] == '1' ||
                       format[start_j] == '2' ||
                       format[start_j] == '3' ||
                       format[start_j] == '4' ||
                       format[start_j] == '5' ||
                       format[start_j] == '6' ||
                       format[start_j] == '7' ||
                       format[start_j] == '8' ||
                       format[start_j] == '9' ||
                       format[start_j] == 'b' ||
                       format[start_j] == 'd' ||
                       format[start_j] == 'x' ||
                       format[start_j] == 'c' ||
                       format[start_j] == 's' ){
                    start_j ++;
                }
            }
        }
        else{
            buf[start_i] = format[start_j];
            start_i ++;
            start_j ++;
        }

        //EOF checking
        buf[start_i] = '\0';
    }

}


#if 0
int my_vsprintf(char *buf, char *format, va_list ap){

    int n = getStrLen(buf);

    //Array to store options
    char options[MAXOPTIONS];
    int  widths[MAXOPTIONS];
    int numOfSigns = getOptions(format, options, widths);
    
    //DEBUGGING
    /*for (int i = 0; i < numOfSigns; i++){*/
        /*printf("option: %c, width: %d\n", options[i], widths[i]);*/
    /*}*/
    if (numOfSigns < 0) return -1;
    //If no args
    if (numOfSigns == 0){
        int i = 0;
        while (format[i] != '\0' && i < (n-1)){
            buf[i] = format[i];
            i++;
        }
        buf[i] = '\0';
        return i;
    }

    //Strings need to plug into format later
    char bufs[MAXOPTIONS][MAXWIDTH];
    //Check each args
    for (int i = 0; i < numOfSigns; i++){
        //Three cases => char, string, integer(signed, unsigned)
        switch(options[i]){
            case 'c':
                bufs[i][0] = (char)va_arg(ap, int);
                bufs[i][1] = '\0';
                break;
            case 's':
                strCopy(bufs[i], va_arg(ap, char*));
                break;
            case 'd':
                inttodec(bufs[i], MAXWIDTH, va_arg(ap, int), widths[i]);
                break;
            case 'b':
                inttobin(bufs[i], MAXWIDTH, va_arg(ap, unsigned), widths[i]);
                break;
            case 'x':
                inttohex(bufs[i], MAXWIDTH, va_arg(ap, unsigned), widths[i]);
                break;
            default:
                break;
        }
    }


}
#endif


/***************************************************/
/*********** snprintf implementation ***************/
/***************************************************/
int my_snprintf(char *buf, int n, char *format, ...){
    //Initial error checking
    //if ((int)sizeof(buf) != n) return -1;
    if (n <= 0 || buf == 0 || format == 0) return -1; 


    //Initial error checking
    //if ((int)sizeof(buf) != n) return -1;
    if (n <= 0 || buf == 0 || format == 0) return -1;   

    //Array to store options
    char options[MAXOPTIONS];
    int  widths[MAXOPTIONS];
    int numOfSigns = getOptions(format, options, widths);
    
    //DEBUGGING
    /*for (int i = 0; i < numOfSigns; i++){*/
        /*printf("option: %c, width: %d\n", options[i], widths[i]);*/
    /*}*/
    if (numOfSigns < 0) return -1;
    //If no args
    if (numOfSigns == 0){
        int i = 0;
        while (format[i] != '\0' && i < (n-1)){
            buf[i] = format[i];
            i++;
        }
        buf[i] = '\0';
        return i;
    }

    //Strings need to plug into format later
    char bufs[MAXOPTIONS][MAXWIDTH];
    //Consider args
    va_list ap;

    //Check each args
    va_start(ap, format);
        for (int i = 0; i < numOfSigns; i++){
            //Three cases => char, string, integer(signed, unsigned)
            switch(options[i]){
                case 'c':
                    bufs[i][0] = (char)va_arg(ap, int);
                    bufs[i][1] = '\0';
                    break;
                case 's':
                    strCopy(bufs[i], va_arg(ap, char*));
                    break;
                case 'd':
                    inttodec(bufs[i], MAXWIDTH, va_arg(ap, int), widths[i]);
                    break;
                case 'b':
                    inttobin(bufs[i], MAXWIDTH, va_arg(ap, unsigned), widths[i]);
                    break;
                case 'x':
                    inttohex(bufs[i], MAXWIDTH, va_arg(ap, unsigned), widths[i]);
                    break;
                default:
                    break;
            }
        }
    va_end(ap);

    //DEBUGGIN
    /*for (int i = 0; i < numOfSigns; i++){*/
        /*printf("%s\n", bufs[i]);*/
    /*}*/

    /*
     * Here we are going to plug strings into format
     */
    replaceWithCorrChars(buf, n, format, bufs);

    return getStrLen(buf);
}


/***************************************************/
/****** printf, assert, scanf implementation *******/
/***************************************************/
void putStr(char *buf){
    int i = 0;
    while (buf[i] != '\0'){
        uart_putc(buf[i]);
        i++;
    }
}

int printf(char *format, ...){
    //Initial error checking
    //if ((int)sizeof(buf) != n) return -1;
    if (format == 0) return -1; 


    //Define empty array at the beginning
    char buf[MAXSTRINGLENGTH];
    //Clear buf
    for(int i = 0; i < MAXSTRINGLENGTH; i++){
        buf[i] = '\0';
    }
    

    //Length checking
    if (getStrLen(format) > MAXSTRINGLENGTH - 1) return 0;

   

    //Array to store options
    char options[MAXOPTIONS];
    int  widths[MAXOPTIONS];
    int numOfSigns = getOptions(format, options, widths);
    

    if (numOfSigns < 0) return -1;
    //If no args
    if (numOfSigns == 0){
        int i = 0;
        while (format[i] != '\0' && i < (MAXSTRINGLENGTH-1)){
            buf[i] = format[i];
            i++;
        }
        buf[i] = '\0';
        putStr(buf);
        return i;
    }

    //Strings need to plug into format later
    char bufs[MAXOPTIONS][MAXWIDTH];
    //Consider args
    va_list ap;

    //Check each args
    va_start(ap, format);
        for (int i = 0; i < numOfSigns; i++){
            //Three cases => char, string, integer(signed, unsigned)
            switch(options[i]){
                case 'c':
                    bufs[i][0] = (char)va_arg(ap, int);
                    bufs[i][1] = '\0';
                    break;
                case 's':
                    strCopy(bufs[i], va_arg(ap, char*));
                    break;
                case 'd':
                    inttodec(bufs[i], MAXWIDTH, va_arg(ap, int), widths[i]);
                    break;
                case 'b':
                    inttobin(bufs[i], MAXWIDTH, va_arg(ap, unsigned), widths[i]);
                    break;
                case 'x':
                    inttohex(bufs[i], MAXWIDTH, va_arg(ap, unsigned), widths[i]);
                    break;
                default:
                    break;
            }
        }
    va_end(ap);
    /*
     * Here we are going to plug strings into format
     */
    replaceWithCorrChars(buf, MAXSTRINGLENGTH, format, bufs);

    //Print out
    putStr(buf);

    return getStrLen(buf);
}




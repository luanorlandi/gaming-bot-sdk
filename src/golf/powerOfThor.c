#define P printf

main(a,b,x,y){
    scanf("%d%d%d%d",&a,&b,&x,&y);
    
    while(1)
        // if(y>b){
        //     P("N");y--;
        // }else if(y<b){
        //     P("S");y++;
        // }
        // if(x<a){
        //     P("E");x--;
        // }else if(x>a){
        //     P("W");x++;
        // }
        y>b ? P("N") & y--:
        y==b ?: P("S") & y++,
        x<a ? P("E") & x++:
        x==a ?: P("W") & x--,
        P("\n");
}

#define P printf
main(a,b,x,y){scanf("%d%d%d%d",&a,&b,&x,&y);while(1)y>b?P("N")&y--:y==b?:P("S")&y++,x<a?P("E")&x++:x==a?:P("W")&x--,P("\n");}
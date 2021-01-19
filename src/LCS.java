import java.util.Random;
import java.util.Scanner;
//      Header file     //

public class LCS {
    public static void main(String[] args){//main function
        Scanner sc = new Scanner(System.in);//input initialization
        System.out.println("Enter the number of array");
        int num = sc.nextInt();//enter the number of array
        int [][] bTable = new int[num][num];
        int [][] cTable = new int[num+1][num+1];
        int [] xValue = new int[num];
        int [] yValue = new int[num];
        long start =0,end=0;
        //initialization

        Random_Generate(xValue);//genearate random values in xValue array
        Random_Generate(yValue);//genearate random values in yValue array
        System.out.println("x");
        for(int i=0;i<num;i++){
            System.out.print(xValue[i]+" ");
        }
        System.out.println();
        System.out.println("y");
        for(int i=0;i<num;i++){
            System.out.print(yValue[i]+" ");
        }
        System.out.println();
        System.out.println();
        //print all elements of two array

        start = System.currentTimeMillis();//timer start

        for(int i=1;i<num;i++)  cTable[i][0]=0;
        for(int i=0;i<num;i++)  cTable[0][i]=0;

        for(int i=1;i<num+1;i++){
            for(int j=1;j<num+1;j++){
                if(xValue[i-1]==yValue[j-1]){
                    cTable[i][j]=cTable[i-1][j-1]+1;
                    bTable[i-1][j-1]=0;
                }
                else if(cTable[i-1][j]>=cTable[i][j-1]){
                    cTable[i][j]=cTable[i-1][j];
                    bTable[i-1][j-1]=1;
                }
                else{
                    cTable[i][j]=cTable[i][j-1];
                    bTable[i-1][j-1]=2;
                }
            }
        }
        //calcuation of cTable and bTable
        end=System.currentTimeMillis();//end timer
        /*
        for(int i=0;i<num;i++) {
            for (int j = 0; j < num; j++) {
                System.out.println("bTable : " + bTable[i][j]);
            }
        }
        System.out.println();
        for(int i=0;i<num;i++) {
            for (int j = 0; j < num; j++) {
                System.out.print(bTable[i][j]+" ");
            }
            System.out.println();
        }
        for(int i=0;i<num+1;i++) {
            for (int j = 0; j < num+1; j++) {
                System.out.print(cTable[i][j]+" ");
            }
            System.out.println();
        }*/
        System.out.println();
        System.out.println("실행 시간 : " + (end - start) + "m 초");//print time
        System.out.println("Length : "+cTable[num][num]);//print length
        //PRINT_LSC(bTable,xValue,num-1,num-1);
    }
    //print longest common subsequence recursively -> foward order
    public static void PRINT_LSC(int[][] bTable,int [] xValue, int i, int j){
        if(i==0||j==0) return ;
        if(bTable[i][j]==0) {
            PRINT_LSC(bTable,xValue,i-1,j-1);
            System.out.print(xValue[i]+"    ");
        }
        else if (bTable[i][j]==1) PRINT_LSC(bTable,xValue,i-1,j);
        else if (bTable[i][j]==2) PRINT_LSC(bTable,xValue,i,j-1);
    }

    //get array -> fill random numbers
    public static void Random_Generate(int [] array){
        for(int i=0;i<array.length;i++){
            array[i]=(int)(Math.random()*100.0);
        }
    }
}

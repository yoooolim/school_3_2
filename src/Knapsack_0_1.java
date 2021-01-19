import java.util.Scanner;

public class Knapsack_0_1 {
    static int max(int a,int b){return (a>b)?a:b;}

    public static void main(String[] args){
        Scanner sc = new Scanner(System.in);
        System.out.print("choose your 'n' : "); //n값 입력
        int n = sc.nextInt();
        int []v = new int[n];
        int []w = new int[n];

        for(int i=0;i<n;i++){//무게 초기화
            w[i]=i+1;
        }
        generateNum(n,v);

        System.out.print("weigth: { ");
        for(int i=0;i<n;i++){
            if(i==n-1)System.out.print(w[i]);
            else System.out.print(w[i]+", ");
        }
        System.out.println(" }");

        System.out.print("value: { ");
        for(int i=0;i<n;i++){
            if(i==n-1)System.out.print(v[i]);
            else System.out.print(v[i]+", ");
        }
        System.out.println(" }\n");

        int W=(int)Math.floor(Math.pow(n,2)/2);

        int [][] Thief = new int [n+1][W+1];
        int k=0;
        //System.out.println(" < B Table > ");
        long start=0,end=0;
        start = System.currentTimeMillis();
        for(int i=0;i<=n;i++){
            for(int j=0;j<=W;j++){
                if(i==0||j==0) Thief[i][j]=0; //초기화 (기본 값 채우기)
                else if(w[i-1]<=j) // 물건 i의 무게가 남은 배낭 용량을 넘지 않는다면
                //물건 i를 배낭에 담은 경우와 담지 않았을때를 비교
                //배낭에 담는 것이 더 가치가 있으면 그 값을 대입
                //담지 않는 것이 더 가치가 있으면 위의 값을 그대로 가져옴
                {
                    Thief[i][j] = max(v[i - 1] + Thief[i - 1][j - w[i - 1]], Thief[i - 1][j]);
                }
                else // 물건의 무게가 남은 용량을 초과한 경우
                    Thief[i][j]=Thief[i-1][j]; //위의 값을 그대로 가져옴
                //System.out.print(Thief[i][j]+"   ");
            }
            //System.out.println();
        }
        end=System.currentTimeMillis();

        System.out.println("optimal solution : "+ Thief[n][W]);
        System.out.println("실행 시간 : " + (end - start) + "m 초");//print time
    }

    static void generateNum(int n, int v[]){//난수 생성
        for(int i=0;i<n;i++){
            v[i] = (int)(Math.random()*2*n+1);//1~2n 사이의 값을 배열에 대입
        }
    }
}

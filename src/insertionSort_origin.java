import java.util.ArrayList;
import java.util.Random;
import java.util.Scanner;

public class insertionSort_origin {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        System.out.println("select option");
        System.out.println("0:ascending[best case] / others:descending[worst case]\n");
        int option = sc.nextInt();
        System.out.println("Enter size of input");
        int inputNum = sc.nextInt();

        ArrayList<Integer> input = new ArrayList<>();

        int innerloopnum1 = 0;
        int innerloopnum2 = 0;

        if(option==0){
            for(int i=0;i<inputNum;i++){
                input.add(i);
            }
        }
        else{
            for(int i=0;i<inputNum;i++){
                input.add(inputNum-i);
            }
        }

        System.out.println("input : " + input);
        //start timer
        long start = System.currentTimeMillis();
            /* 만약 input 배열의 크기가 1 이하라면 sort를
            할 필요가 없기 때문에 1 초과일 때만 들어온다.*/
        if (input.size() > 1) {
            //key값을 하나씩 늘려가며 확인한다.
            for (int i = 1; i < input.size(); i++, innerloopnum1++) { //inner loop test 1
                int key = input.get(i);
                int j = i - 1;
                //key값보다 작은 index의 값들을 비교한다.
                //key의 값이 전의 index 배열보다 작다면
                while (j >= 0 && (key < input.get(j))) {
                    input.set(j + 1, input.get(j));
                    j--;
                    innerloopnum2++;
                }
                input.set(j + 1, key);
                innerloopnum2++;
                /*key의 값이 전의 index 배열 보다 작다면
                아무 일도 일어나지 않고 계속 탐색한다.*/
            }
        }
        //stop timer
        long end = System.currentTimeMillis();
        System.out.println("output : " + input);
        System.out.println("inner loop test 1 접근 횟수 : " + innerloopnum1);
        System.out.println("inner loop test 2 접근 횟수 : " + innerloopnum2);
        System.out.println("실행 시간 : " + (end - start) + "m 초");
        System.out.println("==============================\n");
    }
}

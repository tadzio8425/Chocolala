
import java.lang.Math.*;
import java.util.Arrays;

public class DynamicProgrammingKnapsackAlgorithm implements KnapsackAlgorithm {

	@Override
	public boolean[] solveKnapsack(int[] utilities, int[] weights, int maxWeight) {
		// TODO: Step one find max utility
		int n = weights.length + 1;
		int p = maxWeight + 1;
		
		int[][] matriz = new int[n][p];
		
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < p; j++) {
				if(i == 0) {
					matriz[i][j] = 0;
				}
				else if(weights[i] > j){
					matriz[i][j] = matriz[i-1][j];
				}
				else {
					Math.max(matriz[i-1][j], matriz[i-1][j-weights[i-1]]+utilities[i-1]);
				}
			}
		}
		
		
		// TODO: Step two: traceback to find subset
		boolean[] answer = new boolean [n];
		Arrays.fill(answer, false);
		
		int i = n;
		int j = p;
		
		while(i > 0) {
			if((weights[i-1] <= j) && (matriz[i][j] == matriz[i-1][j-weights[i-1]] + utilities[i-1])) {
				answer[i] = true;
				j = j-weights[i-1];
				i = i-1;
			}
			else {
				matriz[i][j] = matriz[i-1][j];
				i = i-1;
			}
		}
		
		return answer;
	}

}

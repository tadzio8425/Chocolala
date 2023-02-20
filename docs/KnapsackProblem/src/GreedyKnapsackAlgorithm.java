import java.util.Arrays;

public class GreedyKnapsackAlgorithm implements KnapsackAlgorithm {

	@Override
	public boolean[] solveKnapsack(int[] utilities, int[] weights, int maxWeight) {
		int n = utilities.length;
		boolean[] answer = new boolean [n];
		Arrays.fill(answer, false);
		int totalWeight = 0;
		for(int i=0;i<n;i++) {
			if(totalWeight+weights[i]<=maxWeight) {
				answer[i] = true;
				totalWeight+=weights[i];
			}
		}
		return answer;
	}

}

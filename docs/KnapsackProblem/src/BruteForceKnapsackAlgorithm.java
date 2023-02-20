import java.util.Arrays;

public class BruteForceKnapsackAlgorithm implements KnapsackAlgorithm {

	@Override
	public boolean[] solveKnapsack(int[] utilities, int[] weights, int maxWeight) {
		boolean [] subset = new boolean [utilities.length];
		Arrays.fill(subset, false);
		return solveKnapsack(utilities, weights, maxWeight, subset, 0);
	}

	private boolean[] solveKnapsack(int[] utilities, int[] weights, int maxWeight, boolean [] subset, int nextIndex) {
		if(nextIndex==utilities.length) {
			int w = calculateSumSubset(weights,subset);
			if(w>maxWeight) return null;
			return Arrays.copyOf(subset, subset.length);
		}
		boolean [] answerF = solveKnapsack(utilities, weights, maxWeight,subset,nextIndex+1);
		subset[nextIndex] = true;
		boolean [] answerT = solveKnapsack(utilities, weights, maxWeight,subset,nextIndex+1);
		subset[nextIndex] = false;
		if(answerT == null ) return answerF;
		else if (answerF == null) return answerT;
		int uF = calculateSumSubset(utilities,answerF);
		int uT = calculateSumSubset(utilities,answerT);
		if(uF<uT) return answerT;
		return answerF;
	}

	private int calculateSumSubset(int[] values, boolean[] subset) {
		int sum = 0;
		for(int i=0;i<values.length;i++) {
			if(subset[i]) sum+=values[i];
		}
		return sum;
	}

}

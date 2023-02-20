
public interface KnapsackAlgorithm {
	/**
	 * Main interface for algorithms solving the knapsack problem
	 * @param utilities array with the utility of each object
	 * @param weights array with the weight of each object. PRE: weights.length=utilities.length=N
	 * @param maxWeight weight of the knapsack
	 * @return boolean [] array of length N, representing the subset of objects that will be carried on
	 * to maximize the utility. for 0<=i<N, subset[i] == true means that the object i will be carried.
	 */
	public boolean [] solveKnapsack(int [] utilities, int [] weights, int maxWeight);
}

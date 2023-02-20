import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class KnapsackExample {

	/**
	 * Main method for the Knapsack example. Receives test cases as standard input and write to standard output.
	 * See instructions.txt for further details 
	 * @throws Exception if the input can not be read
	 */
	public static void main(String[] args) throws Exception {
		KnapsackExample instance = new KnapsackExample();
		instance.solveProblems();
	}

	public void solveProblems() throws IOException {
		KnapsackAlgorithm algorithm = new DynamicProgrammingKnapsackAlgorithm();
		//KnapsackAlgorithm algorithm = new GreedyKnapsackAlgorithm();
		try (InputStreamReader is=new InputStreamReader(System.in);
				BufferedReader br = new BufferedReader(is);) {
			String line = br.readLine();
			
			for(int cases=0;line!=null  && line.length()>0 && !"0 0".equals(line);cases++) {
				int [] utilities;
				int [] weights;
				int maxWeight;
				try {
					String [] dataStr = line.split(" ");
					int n = Integer.parseInt(dataStr[0]);
					maxWeight = Integer.parseInt(dataStr[1]);
					line = br.readLine();
					dataStr = line.split(" ");
					if(n!=dataStr.length) throw new IOException("Array size does not match for test case: "+(cases+1));
					utilities = new int [n];
					for(int i=0;i<n;i++) utilities[i] = Integer.parseInt(dataStr[i]);
					line = br.readLine();
					dataStr = line.split(" ");
					if(n!=dataStr.length) throw new IOException("Array size does not match for test case: "+(cases+1));
					weights = new int [n];
					for(int i=0;i<n;i++) weights[i] = Integer.parseInt(dataStr[i]);
				}  catch (NumberFormatException e) {
					throw new IOException("Error parsing case: "+(cases+1), e);
				}
				boolean [] answer = algorithm.solveKnapsack(utilities, weights, maxWeight);
				int totalUtility = 0;
				int subsetSize = 0;
				for(int i=0;i<answer.length;i++) {
					if(answer[i]) {
						if(subsetSize>0) System.out.print(" ");
						System.out.print(i);
						totalUtility+=utilities[i];
						subsetSize++;
					}
				}
				System.out.println();
				System.out.println(totalUtility);
				line = br.readLine();
			}
		}
		
	}

}

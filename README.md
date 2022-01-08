# 2D-Shooter-MachineLearning

<br>

**Introduction**
---
While it has been done before, there has been less research in the application of reinforcement learning (RL) to <br>
modern computer games, besides from games like Doom, Minecraft, StarCraft, Tetris, and car racing games.<br>
More often research using RL goes to robotics and multi-agent systems (MAS).<br>
Shooter games have some common features to these fields such as agents having the ability to sense and act to their<br>
surroundings, and complex continuous movements.<br>
Therefore, I found the affects of RL in an 2D Shooter enviroment an interesting area to research.<br>
I will attempt to combine both RL and genetic algorithms (GA) and apply them to my 2D shooter game.<br>
A bot artificial intelligence (AI) generally consists of pathfinding, picking up and using items within<br>
the enviroment and various styles of combat.<br>
Bot AI in commercial games generally uses rule-based systems like, state machines, behaviour trees, etc. These<br>
techniques are typically time-consuming and require fine-tuning of parameters. RL is an interesting algorithm to compete with these systems.<br>
RL could be applied with different parameters and used to generate different bots.<br>
<br>
The aim of this reasearch paper is to see how well I can use RL to learn a bot basic AI behaviours.<br>
To do this I will split the task up in to two major tasks; one being navigation, exploring the environment and item collection.<br>
And the second task being the combat aspect.

<br>

**More about reinforcement learning and genetic algorithms**
---
#### Reinforcement Learning
RL is a machine learning technique where an agent learns trough experience. The agents preforms an action within an environment, which is interpreted into a reward and a representation of the state, which are then criticized trough negative and positive rewards based on a reward function from within the program, to tell the agent how good its action was and how it should should adjust accordingly. 
Today there are several RL algorithms including TD, Q-learning, and Saras. My AI is trained using Q-learning, The main purpose of all the algorithms is to find an optimal policy. The policy is a mapping between states and actions, it provides the path the agents should follow to maximize the reward function. This is simillar to processes that occur in nature; For example our brains are hard wired to interpret signals such as pain and hunger as negative renforcements, and pleasure and food as positive onces, and this is exactly how our RL agent works. When our agent eats it will be rewarded for taking that action, but when our agent suffers from hunger it will die and will be punished for its most resent actions which will include not eating.

#### Genetic Algorithms
GA is a type of optimization algorithm based on natural selection, Genetic algorithms is one branch of the larger class evolutionary algorithms (EA). They imitate the biological processes of reproduction and natural selection to solve for the ‘fittest’ solutions. Like in evolution, many of the processes involve randomness, however GA allows one to set the level of randomization and the level of control. They raquire no extra information about the given problem, this allows them to find solutions to problems that other optimization methods cannot handle due to lack of continuity, linearity, or other.
GA work by starting from an initial generation that is then evaluated trough a user defined function. From there subsequent generations will be generated trough *selection*, *crossover*, and *mutation*.
###### **Selection**
Each part of the genetic algorithm has several different ways that it can be executed.<br>
For the selection part I tried the 3 following techniques.
###### Fitness proportionate selection

<br>

```c++
//Natural Selection
//Code that selects a parent to inherent the brain from,
//agents with higher fitness will have more chance to pass on their brain than agents with lower fitness
//Fitness proportionate selection
Elite::FMatrix* Population::SelectParentFPS(const float sum) const
{
	const float rand{ randomFloat(sum) };

	float runningSum{};

	for (size_t i{0}; i < m_Bots.size(); ++i)
	{
		runningSum += m_Bots[i]->GetFitness();

		if (runningSum >= rand)
			return m_Bots[i]->GetBotBrain();
	}
	return nullptr;
}
```
###### Stochastic universal sampling
```c++
//Stochastic universal sampling
void Population::SelectParentSUS() const
{
	std::vector<float> pointers{};
	auto num{ m_Bots.size() }; //Number of offspring to keep
	auto dist = num / static_cast<float>(num);
	auto start = randomFloat(0, dist);
	for (size_t i{0}; i < num; ++i)
	{
		pointers.push_back(start + static_cast<float>(i) * dist);
	}

	for (size_t i{0}; i < pointers.size(); ++i)
	{
		auto j = 0;
		while (CalculateFitnessSum(0, j) < pointers[i])
		{
			j++;
		}
		m_Bots[i]->SetBotBrain(*m_Bots[j]->GetBotBrain());
	}
}
```
###### Tournament selection
```c++
//Tournament selection
QBot* BestBot = *std::max_element(m_Bots.begin(), m_Bots.end());
```

###### **Crossover**

###### **Mutation**

<br>

[Back to top](#readme)

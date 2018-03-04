using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
/* Written by Salvador Gutierrez 
 * This is a simple console app for keeping track of bowling scores
 * It allows you to input the scores to a traditional game of 10 frames
 * and comes with a suite of tests. 
 */
namespace BowlingApp
{
    public class App
    {
        //Field Vars
        private static Dictionary<int, int[]> players = new Dictionary<int, int[]>();
        static void Main(string[] args)
        {
            int result;
            int numOfPlayers;
            //Menu 
            StringBuilder menu = new StringBuilder(
                "[0] exit\n" +
                "[1] input\n" +
                "[2] randomize/simulate\n"
                );
            Console.WriteLine(menu);
            //Switch for Menu 
            int.TryParse(Console.ReadLine(), out result);
            switch(result)
            {
                case 0:
                    Environment.Exit(0);
                    break;
                case 1:
                    Console.WriteLine("\nEnter number of players: ");
                    int.TryParse(Console.ReadLine(), out numOfPlayers);
                    readInputScores(numOfPlayers);
                    break;
            }
            returnScore();
            Console.ReadLine();


        }

        /* 
         * Prints out message to input score 
         */
        private static int turnDisplay(int i, int j, ref int score)
        {
            Console.WriteLine("\nPlayer " + (i + 1) + " Bowl # " + (j + 1) + " score:");
            int.TryParse(Console.ReadLine(), out score);
            return score;
        }
        /* 
         * Helper for checking input is within correct range for turn in frame 
         */
        private static void validateHelper(ref int score, int j, int prevScore)
        {
            while (!validate(score, j, prevScore))
            {
                Console.WriteLine("Incorrect amount, please re-enter score: ");
                int.TryParse(Console.ReadLine(), out score);
            }
        }
        /*
         * Calculates the score frame by frame. Protection should be private (set to public for testing)
         */
        public static int scoreAdder(int[] arr)
        {
            int frameScore = 0;
            int totalScore = 0;
            for(int i = 0; i < arr.Length - 2; i+=2)
            {
                //If element is a 10 and less than pos 17
                if(arr[i] == 10 && i < 18)
                {
                    //If a strike is bowled, the turn 2 of the frame is zero
                    //That's why we skip i+1 and i+3
                    if(i == 16)
                        frameScore = arr[i] + arr[i + 2] + arr[i + 3];
                    else
                        frameScore = arr[i] + arr[i + 2] + arr[i+4];
                }
                //If strike at pos 18
                else if(arr[i] == 10 && i ==18)
                {
                    frameScore = arr[i] + arr[i + 1] + arr[i + 2];
                }
                //Check if spare
                else if(i < 20 && arr[i] + arr[i+1] == 10)
                {
                    frameScore = arr[i] + arr[i + 1] + arr[i+2];
                }
                else
                {
                    frameScore = arr[i] + arr[i + 1];
                }
                totalScore += frameScore;
                Console.Write("Frame # " + (i+1)+" Bowled:" + arr[i] + " " + arr[i + 1] + "\n");
            }
            return totalScore;
        }
        /* 
         * Prints out the score for each player 
         */
        private static void returnScore()
        {
            //foreach player in the dictionary
            foreach(KeyValuePair<int, int[]> el in players)
            {
                //Display game 
                Console.WriteLine("Player " + el.Key + "'s score");
                Console.WriteLine("Total = " + scoreAdder(el.Value) + "\n");
            }
        }


        /* 
         * Logic for checking input validation 
         */
        private static bool validate(int num, int turn, int prev)
        {
            if (num > 10)
                return false;
            if (num == 10 && turn % 2 != 0)
            {
                if (turn > 18)
                {
                    return true;
                }
                else
                    return false;
            }
            if (num < 0)
                return false;
            if (prev + num > 10)
            {
                if (turn > 18)
                    return true;
                else
                    return false;
            }
            else
                return true;
        }

        /* 
         * Reads user input. 
         */
        private static void readInputScores(int numOfPlayers)
        {

           for(int i = 0; i < numOfPlayers; i++)
            {
                //Vars
                int[] scores = new int[21];
                int score = 0;
                int prevScore = 0;

                //Clear screen
                Console.Clear();

                //Populate Values Loop
                for(int j = 0; j < 20; j++)
                {
                    //Modify print to reflect natural ordering 
                   turnDisplay(i, j,ref score);

                    //Keep track of previous for spares
                    if(j%2 == 1)
                        prevScore = scores[j - 1];
                    else
                        prevScore = 0;
                    validateHelper(ref score,j,prevScore);

                    //If Strike and not in last turn 
                    if(score == 10 && j < 18)
                    {
                        //Skip to next frame 
                        scores[j] = score;
                        j++;
                    }

                    //Check if turn 19 (j == 18) is strike
                    else if(score == 10 && j == 18)
                    {
                        //Don't skip 
                        scores[j++] = score;
                        //Logic for extra bowl 
                        //Modify print to reflect natural ordering 
                        turnDisplay(i, j, ref score);
                        validateHelper(ref score, j, prevScore);

                        //Turn 20 can be anything, including a strike 
                        scores[j++] = score;
                        //Turn 21
                        turnDisplay(i, j,ref score);
                        validateHelper(ref score, j, prevScore);

                        //Check if 21 is spare
                        if (score + scores[j-1] == 10)
                        {
                            //Stuff happens 
                            scores[j] = score;
                        }
                        //Check if 21 is strike
                        if(score == 10)
                        {
                            //If it is a strike, it means 20 is a strike 
                            //so do add scores correctly for that 
                            scores[j] = score;
                        }

                    } 
                    //Check if turn 19 is spare
                    else if(j > 18 && prevScore + score == 10)
                    {
                        scores[j] = score;
                        j++;
                        turnDisplay(i, j, ref score);
                        validateHelper(ref score, j, prevScore);
                        scores[j] = score;
                    }
                    else
                    {
                        scores[j] = score;
                    }
                    //Clear Console
                    Console.Clear();
                }
                players.Add(i, scores);
            }
        }
    }
}

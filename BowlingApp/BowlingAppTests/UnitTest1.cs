using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace BowlingAppTests
{
    [TestClass]
    public class UnitTest1
    {
        //Input is hard to test (was done manually), I would like to think about my function design next time 
        //To make it so I can automate it's testing.
        //These tests cover the adder function 
        //INVALID INPUT is tested in the readInput function so by now everything is clean
        [TestMethod]
        public void GutterBalls()
        {
            int[] arr = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            int result = BowlingApp.App.scoreAdder(arr);
            Assert.AreEqual(0, result);
        }
        [TestMethod]
        public void RegularBowls()
        {
            //The input function won't allow an extra roll if the last turn doesn't have a spare or strike
            int[] arr = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0 };
            int result = BowlingApp.App.scoreAdder(arr);
            Assert.AreEqual(80, result);
        }
        [TestMethod]
        public void RegularWithStrikeAndSpare()
        {
            int[] arr = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 10, 4, 6 };
            int result = BowlingApp.App.scoreAdder(arr);
            Assert.AreEqual(92, result);
        }
        [TestMethod]
        public void RegularWithStrikeNoSpare()
        {
            int[] arr = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 10, 4, 4 };
            int result = BowlingApp.App.scoreAdder(arr);
            Assert.AreEqual(90, result);
        }
        [TestMethod]
        public void RegularWithSpareAndStrike()
        {
            int[] arr = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 10 };
            int result = BowlingApp.App.scoreAdder(arr);
            Assert.AreEqual(92, result);
        }
        [TestMethod]
        public void RegularWithSpareNoStrike()
        {
            int[] arr = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6 };
            int result = BowlingApp.App.scoreAdder(arr);
            Assert.AreEqual(88, result);
        }
        [TestMethod]
        public void PerfectGame()
        {
            //The input function zeros the next turn in the frame if a strike is bowled
            //But it knows not to if at the last frame
            int[] arr = { 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 10, 10 };
            int result = BowlingApp.App.scoreAdder(arr);
            Assert.AreEqual(300, result);
        }
        [TestMethod]
        public void SprinkledSpares()
        {
            int[] arr = { 4, 6, 4, 4, 3, 7, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6 };
            int result = BowlingApp.App.scoreAdder(arr);
            Assert.AreEqual(100, result);
        }
        [TestMethod]
        public void SprinkledStrikes()
        {
            //The input function zeros the next turn in the frame if a strike is bowled
            //But it knows not to if at the last frame
            int[] arr = { 4, 4, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 10, 0, 4, 4, 0 };
            int result = BowlingApp.App.scoreAdder(arr);
            Assert.AreEqual(238, result);
        }
        

    }
}

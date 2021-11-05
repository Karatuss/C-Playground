# Socket Programming
## Summary
Simple socket programming.

I apply **a threading algorithm** to control multiple clients simultaneously.
Especially, I focus on implemeting the algorithm for **the Readers-Writers Problem**.
There are three ways to solve the Readers-Writers Problem, so I selected one of them having a feature of *FAIRNESS*.

## Structure
- launcher : A manager might call this program to deal with both server and client program for convenience.
- server   : Server interface. Utilities for server are inside here.
- client   : Client interface. Utilities for client are inside here.

It is prihibited to access **directly** with calling functions from outside of launcher.
Please aware about this information.

# Reference
1. Readers-Writes Problem
   - https://en.wikipedia.org/wiki/Readers–writers_problem

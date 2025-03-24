#!/bin/bash

# Use pgrep to find the process ID of a running process named "./child"
CHILD_PID=$(pgrep -f "./child")

# Check if the CHILD_PID variable is empty, indicating no such process was found
if [ -z "$CHILD_PID" ]; then
  echo "Child process not found. Ensure parent is running first."
  exit 1  # Exit with an error code if the child process is not found
fi

# Notify the user about the SIGTSTP signals being sent every 3 seconds
echo "Sending SIGTSTP to child process (PID: $CHILD_PID) every 3 seconds..."

# Loop five times, sending a SIGTSTP signal every 3 seconds
for i in {1..5}; do
  sleep 3  # Wait for 3 seconds
  echo "Sending SIGTSTP to $CHILD_PID"  # Log message indicating signal is being sent
  kill -SIGTSTP "$CHILD_PID"  # Send the SIGTSTP signal to the child process
done

# Wait for 3 seconds after the last SIGTSTP signal
sleep 3
echo "Done sending signals. Now sending SIGTERM to child ($CHILD_PID)."
kill -SIGTERM "$CHILD_PID"  # Send SIGTERM signal to terminate the child process

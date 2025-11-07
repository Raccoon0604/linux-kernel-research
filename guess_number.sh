
#!/usr/bin/env bash
MIN=1
MAX=100

# Generate random number (1..100)
TARGET=$(( (RANDOM % (MAX - MIN + 1)) + MIN ))
TRIES=0

echo "Welcome to the Guess Number Game!"
echo "I have chosen a number between ${MIN} and ${MAX}."
echo "Enter your guess and press Enter. Type q or quit to exit."

while true; do
  read -p "Your guess: " GUESS

  # Exit support
  if [[ "$GUESS" == "q" || "$GUESS" == "quit" ]]; then
    echo "Game exited. The correct number was: ${TARGET}"
    exit 0
  fi

  # Trim spaces
  GUESS="$(echo -e "${GUESS}" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')"

  # Check empty input
  if [[ -z "$GUESS" ]]; then
    echo "Please enter a number (or q to quit)."
    continue
  fi

  # Integer validation
  if ! [[ "$GUESS" =~ ^[0-9]+$ ]]; then
    echo "Please enter a valid positive integer (1-100), or q to quit."
    continue
  fi

  # Convert to number and count
  GUESS_NUM=$((GUESS + 0))
  TRIES=$((TRIES + 1))

  # Range check
  if (( GUESS_NUM < MIN || GUESS_NUM > MAX )); then
    echo "Please enter a number between ${MIN} and ${MAX} (you entered ${GUESS_NUM})."
    continue
  fi

  # Compare
  if (( GUESS_NUM < TARGET )); then
    echo "Too low! Try again. (Attempts: ${TRIES})"
  elif (( GUESS_NUM > TARGET )); then
    echo "Too high! Try again. (Attempts: ${TRIES})"
  else
    echo "Congratulations! You guessed it! The number was ${TARGET}."
    echo "Total attempts: ${TRIES}."

    # Play again option
    while true; do
      read -p "Play again? (y/n): " ANS
      case "$ANS" in
        y|Y|yes|Yes)
          TARGET=$(( (RANDOM % (MAX - MIN + 1)) + MIN ))
          TRIES=0
          echo "New game started! I have chosen a number between ${MIN} and ${MAX}."
          break
          ;;
        n|N|no|No)
          echo "Thanks for playing!"
          exit 0
          ;;
        *)
          echo "Please enter y or n."
          ;;
      esac
    done
  fi
done


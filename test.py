file1_path = 'correct.txt'
file2_path = 'output.txt'

with open(file1_path, 'r') as file1, open(file2_path, 'r') as file2:
    for line_number, (line1, line2) in enumerate(zip(file1, file2), start=1):
        if line1 != line2:
            if (line1[-1] == '\n'): line1 = line1[0:-1]
            if (line2[-1] == '\n'): line2 = line2[0:-1]

            print(f"Line {line_number}:")
            print(f"Correct: '{line1}'")
            print(f"Got: '{line2}'")
            print()
            break

import random


def read_dna_file(filename):
    with open(filename, "r") as file:
        return file.read().strip()


def select_random_substring(dna, length=10000):
    """Selects a random substring of the given length from the DNA sequence."""
    if len(dna) < length:
        raise ValueError("DNA sequence is shorter than the desired substring length.")

    start_index = random.randint(0, len(dna) - length)
    return dna[start_index : start_index + length]


def flip_characters(dna_substring, flip_prob=0.1):
    """Flips each character in the substring with a given probability."""
    flipped = []
    for char in dna_substring:
        if random.random() < flip_prob:
            if char == "A":
                flipped.append("C")
            elif char == "C":
                flipped.append("G")
            elif char == "G":
                flipped.append("T")
            elif char == "T":
                flipped.append("A")
            else:
                flipped.append(char)
        else:
            flipped.append(char)

    return "".join(flipped)


def write_pattern_file(filename, pattern):
    with open(filename, "w") as file:
        file.write(pattern)


def main():
    input_filename = "dna_10000.txt"
    output_filename = "pattern.txt"
    substring_length = 9999

    dna_sequence = read_dna_file(input_filename)
    random_substring = select_random_substring(dna_sequence, substring_length)
    modified_pattern = flip_characters(random_substring, flip_prob=0.1)
    write_pattern_file(output_filename, modified_pattern)
    print(f"Process completed. Modified pattern written to {output_filename}")


if __name__ == "__main__":
    main()

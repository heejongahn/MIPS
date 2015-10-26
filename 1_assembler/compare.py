filename = input("Filename:\t")
correct = True
with open(filename, "r") as f:
    with open(filename[:-2] + "_answer.o", "r") as f2:
        s1 = f.readline()
        s2 = f2.readline()

        for i in range(len(s1)):
            if s1[i] != s2[i]:
                print ("Wrong at position {}".format(i))
                print ("My file: {}".format(s1[i:i+32]))
                print ("Answer: {}".format(s2[i:i+32]))
                i+=32;
                correct = False

        if correct:
            print ("Correct!")

# Documentation
Generate documents using [Doxygen](https://doxygen.nl)

[Prerequisites](#prerequisites) <br>
[Generating and viewing documentation](#generating) <br>
[Troubleshooting](#troubleshooting) <br>
[Contributing to documentation](#contributing) <br>

---

<a name="prerequisites"></a>
## Prerequisites
* Clone this repository to your local computer. (Running this over ssh should work but will be slow. Running over docker may not work.)
* Install Doxygen
<table>
<tr>
<th> Linux </th>
<th> Mac </th>
</tr>
<tr>
<td>

```bash
sudo apt install doxygen
```

</td>
<td>

```bash
brew install doxygen
```

</td>
</tr>
</table>

<a name="generating"></a>
## Generating and viewing documentation
From the `emphaticsoft/` directory

```bash
cd docs/
./make-docs
```

Once the browser opens, you will see a link to `files` on the webpage, this is propably the best place to start looking aroung the documentation.

<a name="troubleshooting"></a>
## Troubleshooting
If the `make-docs` script does not open up a webpage, you will need to find a way to open `emphaticsoft/docs/Doxyout/html/index.html` to view the documentation.

## Contributing
Use triple slashes `///` before a class or method, and `///<` for inline comments, to have that comment show up in the documentation alongside the thing that you're commenting on.

For example, this code
```c++
/// This is a brief description about a person.
///
/// A longer and more detailed description can go here.
class Person
{
    public:
        Person();

        /// Calculates something.
        ///
        /// Once again, a longer description can be put here.
        ///
        /// @param a description of the first parameter
        /// @param b and of the second
        /// @returns Describe what this method returns
        float CalculateSomething(int a, int b);

        int firstName;  ///< The persons first name
        int lastName;  ///< The persons last name 

    private:
        int age;
};
```
Will generate something like this

![doxygen_example.pdf](https://github.com/EMPHATICSoft/emphaticsoft/files/9075318/doxygen_example.pdf)

More information can be found [here](https://doxygen.nl/manual/docblocks.html) about using doxygen style comments.


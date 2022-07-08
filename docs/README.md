# Documentation
Generated using [Doxygen](https://doxygen.nl)

[Prerequisites](#prerequisites) <br>
[Generating and viewing documentation](#generating) <br>
[Troubleshooting](#troubleshooting) <br>
[Contributing to documentation](#contributing) <br>

---

<a name="prerequisites"></a>
## Prerequisites
* Clone this repository on to your local computer. (Running this over ssh should work but will be slow. Running over docker may not work.)
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

<a name="troubleshooting"></a>
## Troubleshooting
If the `make-docs` script does not open up a webpage, you will need to find a way to open `emphaticsoft/docs/Doxyout/html/index.html` to view the documentation.

## Contributing


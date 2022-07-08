# How to Generate Documentation
[Prerequisites](#prerequisites) <br/>
[How to Run](#how-to-run) <br/>
[Troubleshooting](#troubleshooting)

<a name="prerequisites"/>

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

<a name="how-to-run"/>

## How to Run
From the `emphaticsoft/` directory

```bash
cd docs/
./make-docs
```

<a name="troubleshooting"/>

## Troubleshooting
If the `make-docs` script does not open up a webpage, you will need to find a way to open `emphaticsoft/docs/Doxyout/html/index.html` to view the documentation.

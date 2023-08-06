<html>
  <body>
  <h1>RLS v2</h1>
  <h3>Rainbow-LS-CPP</h3>

  <h2>About</h2>
  <p>Have you ever been bored of the boring old ls command always showing you the same dull colors?</p>
  <p>Well I have a solution for you! RLS!</p>
  <p>With RLS you can see your directories in style. Rainbow style.</p>
<br>
  <p>This is the second version of the rls command. Now with faster code, smaller binary, and more colors. </p>
  <h3>Features:</h3>
  <p>    - Lists directories.</p>
  <p>    - As a rainbow.</p>
  <h3>Improvements:</h3>
  <p>    - Outputs rainbow in both x and y directions. </p>
  <p>    - Rainbow is more visible due to dynamic length. </p>
  <p>    - Is faster than the old Golang and C++v1 rls.</p>
  <p>    - Uses no external libraries. I made the whole thing as small as possible. </p>
  <p>    - When compiled dynamically it is only 20kb.</p>
  <br>

<h2>Feel The Rainbow</h2>
<p align=center>
<img src="https://sophuwu.site/rls/rls2.png" style="width: 40%;">
</p>

<h2>Build and Installtion</h2>

<h3>Build</h3>
<p>Simply clone the repository or download the files. Then you can simply run <b>make build</b> in the root of the repository to build the program. If you have strip or UPX installed you can run <b>make small</b> to shrink the binary. Then simply do <b>make install</b> and <b>make clean</b> to install and clean up the build cache.</p>

<h3>Download</h3>
<p>Go to <a href="https://sophuwu.site/rls">sophuwu.site</a> and download rls. Verify the <a href="https://sophuwu.site/pgp.txt">PGP</a> signature if you like. Then simply copy the downloaded file into /usr/local/bin/</p>

<h3>Alias</h3>
<p>You can add the line <b>alias ls="rls"</b> to your ~/.bashrc to use rls like its ls. If you have any issues with rls then you can simply use <b>dir</b> as the  original ls.</p>
  </body>
</html>

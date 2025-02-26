# Creating a pull request

You can contribute to this project by opening a pull request that will be reviewed by maintainers and integrated into the main repository when the changes made are approved. You can contribute bug fixes, changes to the documentation, or new functionality you have developed.

You can follow these steps to create a pull request for this project:

1. Fork the repository, clone it and create a new branch:

	``` bash linenums="1"
	git clone git@github.com:your-username/nrf9151-connectkit.git
	```

	``` bash linenums="2"
	cd nrf9151-connectkit
	```

	``` bash linenums="3"
	git checkout -b your-new-branch-name
	```

2. Add your changes to the codebase.

3. Commit your changes with your sign-off message:

	``` bash
	git commit --signoff --message 'This is my commit message'
	```

	!!! Tip
		A sign-off message in the following format is required on each commit in the pull request:

		``` { .text .no-copy linenums="1" }
		This is my commit message

		Signed-off-by: First_Name Last_Name <My_Name@example.com>
		```

		This will use your default git configuration which is found in `.git/config` and usually, it is the `username systemaddress` of the machine which you are using.

		To change this, you can use the following commands (Note these only change the current repo settings, you will need to add `--global` for these commands to change the installation default).

		Your name:

		``` bash
		git config user.name "First_Name Last_Name"
		```

		Your email:

		``` bash
		git config user.email "My_Name@example.com"
		```

4. Push your commit to your forked repository:

	``` bash
	git push origin your-new-branch-name
	```

5. Create a pull request. See [GitHub's official documentation] for more details.

[GitHub's official documentation]: https://help.github.com/articles/creating-a-pull-request-from-a-fork/

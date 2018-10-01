# Contributing to Paladin

Everyone is welcome to help further improve Paladin. We don't just need 
developers either. A range of tasks are available that you could work on:-

- C++ Code development (obviously!)
- Testing out features - both on 32 and 64 bit Haiku installs and new releases
- Writing high quality documentation
- Translating the User Interface - See [Paladin on Polygot](https://i18n.kacperkasper.pl/projects/22) for details
- Trying out the Template Projects for yourself (or writing new Templates!)
- Source code style review
- Working on the [Haiku Ports](https://github.com/haikuports/haikuports/tree/master/haiku-apps/paladin) port file - this provides the Paladin app inside HaikuDepot

However you choose to get involved, there are some things you should know...

## Conventions

There are a few conventions we try to adhere to. These are described below.

### License and Copyright

All the code is licensed under the MIT license, and Copyright of the author or recent developer.
That means you can add your name to a code file and the main LICENSE text file when submitting
a pull request. 

See the Coding Conventions below for more information.

### GitHub and GitFlow

GitFlow is used to maintain and contribute commits/patches. In general, best to do this:-

1. Report an issue, or find an issue you want to work on. This gives you an Issue Number
1. Fork the Paladin repository on GitHub, and clone to your desktop
1. Create a new branch off of master called feature-ISSUENUM , where ISSUENUM is your new issue number
1. Make the changes necessary - checking the code style matches as below - then test and commit the change
   - In the commit title, provide a very brief 1 line description followed by ". Fixes #ISSUENUM" - this relates your commit and branch to the issue it is intended to fix.
1. Push the commit to your fork
1. Create a Pull Request on GitHub, with a full description of what it fixes/provides
1. Wait for a Contributor to accept your change (With thanks!), or ask for clarification/changes

That's it! You're now a Paladin developer!

### Code Style

We try to abide by a few general Haiku conventions when coding. They are described below:-

- [General code style conventions](https://www.haiku-os.org/development/coding-guidelines) 
- [Submitting patch guidelines](https://dev.haiku-os.org/wiki/CodingGuidelines/SubmittingPatches)
- GitFlow - We use GitFlow with a couple of minor modifications:-
   - Our in-progress 'develop' branch is called 'master'
   - Our good-quality 'master' branch is called 'release'
   - Our 'feature' branches are called 'feature-ISSUENUM' and are always based on 'master'

## Getting help / making contact

There are several developers or interested parties. See our [GitHub Contributors page](https://github.com/HaikuArchives/Paladin/graphs/contributors)
for details.

### Reporting issues/ideas/feature requests at GitHub

Please feel free to search for and report new bugs or ideas on the [Paladin GitHub Issues Pages](https://github.com/HaikuArchives/Paladin/issues).

### IRC

You can reach the developers on irc.freenode.org at #haiku-3rdparty

### Haiku Forums

If a web based forum is more your style, please visit the [Haiku Forums](https://discuss.haiku-os.org/)
and leave us a message there.

## In Summary

Thanks for considering contributing to the Paladin Project!


![logo](images/PACMAN-logo-FINAL.png)

## Background
---
### MORA
The Modular Open Radio Frequency Architecture (MORA) was developed by DEVCOM as a way to promote interoperability between RF components. It is an extension to the VICTORY standard and pushes forward the Army's goal of modular open architectures.
### Hammer
[Hammer](https://github.com/UpstandingHackers/hammer) is a parser-combinator library where grammars are defined as inline domain-specific languages. Developed by Meredith Patterson, it is the basis on which PACMAN is written.
We recommend watching [The Science of Insecurity](https://www.youtube.com/watch?v=3kEfedtQVOY) as well as [Towards a formal theory of computer insecurity: a language-theoretic approach](https://www.youtube.com/watch?v=AqZNebWoqnc) for more information on Hammer's approach to security. We also suggest listening to Meredith and Sergey Bratus' talk on ['shotgun parsers'](https://www.youtube.com/watch?v=mLc0cwlVe84) for some good background information and use cases of Hammer.
#### Insecure Parsers
Parser security has historically been overlooked in legacy applications. This has led to a number of vulnerabilities regarding incorrect parser implementation. These vulnerabilities often result from attempts to fix parser bugs with external handling, rather than fixing the parser itself. Below we provide a few examples of famous parser failures:

##### [Heartbleed](https://heartbleed.com/)
* Vulnerability within the OpenSSL cryptographic library
* Allows for stealing the information normally protected by SSL/TLS
* Memory read bug
* Bug is in OpenSSL's implementation of the TLS heartbeat extension
* No bounds checking on buffer
* Requests with payload shorter than the asserted length did not conform to TLS protocol grammar, but were parsed anyway

<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/11/Simplified_Heartbleed_explanation.svg/1024px-Simplified_Heartbleed_explanation.svg.png" width="400" height="400"/>

##### [Rosetta Flash](https://www.avira.com/en/blog/understanding-rosetta-flash-vulnerability)
* Takes advantage of websites ability to receive information from other external websites via JSONP
* Uses flash files that have been formatted to only consist of printable characters
* Flash file is provided as callback parameter to external website causing information to leak
* Underlying vulnerability in the flash parser from being too liberal and forgiving

<img src="https://cdn.arstechnica.net/wp-content/uploads/2014/07/rosettaflash_convert-640x256.png" width="640" height="256"/>

##### [Android Master Key](https://resources.infosecinstitute.com/topic/android-master-key-vulnerability-poc/)
* Takes advantage of Android code signing
* Allows attackers to inject malicious code into legitimate Android apps and still present a valid digital signature
* APK files are parsed and verified by a different implementation than the code that eventually loads content from the package
* Parser was too lucrative in how it handled duplicate files and verified signatures

<img src="https://i0.wp.com/securityaffairs.co/wordpress/wp-content/uploads/2013/07/Android-security-master-key-vulnerability.gif?resize=442%2C355" width="500" height="300"/>

### PACMAN
The Parser Combinator for MORA Networks (PACMAN) tool aims to create a secure parsing environment for use in MORA-compliant Radio Frequency (RF) systems. MORA uses MORA Data Message (MDM) packets for communication across its two-bus system. PACMAN is intended to parse these packets in transport in order to ensure they conform to the MORA specification. By outlining a set of standards that MDMs must conform to, PACMAN can formally ensure that packets are bug free, without having to check for edge cases. This also allows error handling to occur at the parser level, as all packets must conform to a specific grammar, reducing the amount of input handling that must occur once the packets reach their destination.

## Getting Started
---
### Installation
After cloning this repository, simply run `source ./configure.sh`. Note that testing was performed on an updated version of Ubuntu 22.04.

### Compilation
To compile, run the following command from the top level folder of this repository:
`make test`
You can then run the program using `./runTests`.

If you wish to continue execution past failed tests, invoke `./runTests` with `'-k'`.

In order to clean up the current environment, type `make clean`.

## Testing Expansion
---
In order to implement additional tests, you must follow the testing structure as it is defined in the Hammer library. The [Hammer User Guide](https://github.com/UpstandingHackers/hammer/wiki/User-guide) is helpful in learning how to use these testing functions. Note that PACMAN currently reads fields and subfields with whitespace delimiters.

Once you have developed your test function, you can edit the `register_parser_tests()` function to include your test. The simplest way to do so is by using the format `g_test_add_func("/ok/NAMEOFTEST", NAMEOFTESTFUNCTION)` (in the case of a test that you want the parser to pass), or `g_test_add_func("/fail/NAMEOFTEST", NAMEOFTESTFUNCTION)` (in the case of a test that you want the parser to fail).

For more information on testing, refer to the Hammer and Glib reference documents.

## Semantic Actions
---
Hammer's semantic actions unlock a world of powerful features that you can use to alter data with your parser. You will notice that there are already some semantic actions in PACMAN for creating unsigned integers out of sequences of digits.

You too can add semantic actions to PACMAN to aid you in altering the input coming through your parser. If you wish to have MDMs appear in hexadecimal format or without delimiters after parsing, that can easily be accomplished through the use of semantic actions.

For more information on some of Hammer's built-in semantic actions as well as syntax for adding more, refer to the Hammer documentation. In short, you can attach a semantic action to any parser you would like, by changing the parser from an H_RULE to an H_ARULE. You must then define the action in a specially named function (see `act_nDigitInt()` for an example). In this function you can operate on the abstract syntax tree (AST) however you would like, and return results in various forms. You can even define custom data types (not demonstrated in this program) for use in your semantic actions.

## Known Issues
---
Currently the PACMAN tool only works with the default Hammer backend, Packrat. Is is unknown why the others result in an error.

PACMAN currently reads MDM fields as integers or characters. This may not conform exactly to how your system creates and processes MDMs. This functionality can be changed to read fields as hex or bits, but will require changes to the PACMAN source code. If such changes are made, feel free to create a separate branch with these changes and create a pull request. Our team will evaluate the changes and integrate them accordingly.

If you read the MORA standard, you will notice that some fields utilize reserved subfields that do not currently hold any information, and are represented by `0` bits. This is to allow for expansion of the MORA standard in the future. Currently, PACMAN does not parse any information about these reserved bits, and assumes they are omitted. For example, for a field with the following subfields:
* Unsigned 16 bit integer
* 8 reserved bits
* Unsigned 8 bit integer

PACMAN would look for the following example info to read in and parse:
`45324 126`
Notice how there are no reserved `0` bits in between the two unsigned integers.
This can easily be changed by including reserved bits in the fields where they are applicable, but will require changes to the PACMAN source code.

PACMAN currently does not parse MDM Type 1 packets. This is due to the complexities that Type 1 presents. In-progress code for Type 1 parsing can be found in the typeOne branch of this repository. Feel free to work with this code and create pull requests for that branch. We hope to eventually incorporate Type 1 parsing into the main PACMAN branch.

## Help Needed
---
There are a few areas in which our team could use the assistance of the open-source community:
* We found it difficult to get a hold of MORA-compliant systems using MDMs for communication. As such, we would like to see any systems you may have that are MORA-compliant so that we can examine the ways in which the packets travel across the system.
* We seek input on changes that should be made to PACMAN in order to identify the most common ways that MDMs appear in MORA systems. As detailed in the Known Issues, we currently process MDMs as sequences of characters and integers. This can be changed to any format that is needed, so we would like to hear what formats the RF community needs in terms of MDM parsing.
* Going alongside the last point, we are open to creating semantic actions so that PACMAN changes the AST of packets that are parsed. These changes would depend on the feedback from developers on how they want packets to look like after parsing.

## Contact
---
You can email the PACMAN team by reaching out to SOSA@riversideresearch.org


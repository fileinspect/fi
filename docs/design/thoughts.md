# Initial Design Thoughts

This page exists to document initial brainstorming and ideation about the `fi`
editor.

## Motivation

Why another editor? Quite frankly, this is primarily because I believe modal
text editing is currently severely under-utilized. While modal editing is still
progressing in some ways (note Kakoune), the continuation of backwards
compatibility with Vi-family keybinds or editing models means that modal text
editing is gated behind a relatively high complexity. When I wish to recommend
editors for friends, peers, co-workers, etc... to use, I cannot recommend in
good faith a modal text editor, as none exist that are easy to use from the
get-go. With this in mind, the `fi` editor should be designed primarily around
its user experience, rather than around technical merits and ideation. In so
doing, I hope to also resolve frustrations that I have experienced with
existing editors, such as a highly fragmented plugin ecosystem, overt amounts
of time spent in configuration and setup, lack of cross-system portability and
synchronization, among other issues.

## Server Architecture

The `fi` editor should be run in a server-client architecture. Right now, the
goal is to use a TCP server rather than IPC systems such as pipes. This is for
several reasons.

First, a TCP server appears to be more easily platform and language agnostic
than IPC systems. Second, this enables use of clients on a remote network
connection, such as SSH port forwarding.

Using a server-client architecture can enable multiple different client
technologies. Especially with integrated development environments in game
engines (e.g. Godot) or on the web, having such extendable clients allows for
using the `fi` editor in more ways than it being a simple terminal editor would
allow.

As the goal of a server-client architecture is to encourage varied clients, the
editor should make the creation of a client as easy as possible, without
limiting its implementation methods. For this goal, perhaps the editor can
provide high level UI structures and elements that the client can then "render"
in its own preferred method. The reason for providing such UI structures and
elements is so that third party plugins that necessitate UI interfaces can be
client agnostic.

If such a UI-rendering client-server architecture is pursued, this should also
be accompanied by plugins that can be loaded or unloaded dynamically. The core
server and connecting clients can negotiate which UI (or more general) features
are supported, and the core server can only run the plugins that are supported
by connected clients.

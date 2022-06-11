# fi

> Modern modal text editing.

This text editor was primarily inspired by not having alternative modal text
editors to recommend to friends, peers, and co-workers. While great modal text
editors such as `(neo)vi/vim` and `kakoune` exist, such editors were needlessly
complex for many people and had a low adoption rate. Most people tended to stay
with non-modal text editors despite a sustained interest in modal text editing.
As such, the motivation for `fi` was initially born.

`fi` has one key guiding principle around its user experience design:

> Comfort > Speed

One common rebuttal against modal text editors such as the `vi` family is that
the limiting factor for text editing is not the *speed* of editing, but instead
figuring out *what* edits must be made. While this point does not yet seem to
be entirely settled, one commonly accepted statement is that the main
bottleneck in a day of developer productivity is usually *not* the speed with
which text editing operations can be carried out.

The primary goal of designing `fi` to have *intuitive* and *comfortable* text
editing is to tackle one major perceived problem in developer productivity,
which is physical endurance. Many developers experience strain or injuries in
their fingers, hands, or forearms from repeated long text editing sessions.
While modal text editing does help alleviate this problem as compared to
non-modal editors that require the frequent use of modifier keys, `fi` hones in
on this aspect of modal text editing as a primary advantage. Less needed keys
for a given operation can often line up with the goal of reduced physical
strain, but when there is a tradeoff between using less keystrokes for faster
editing operations and reducing stretch and strain on finger movements, `fi`
will err towards the side of comfort.

Another important goal for `fi`'s design is to help ease the learning curve for
new users. By abandoning backwards-compatibility with the `vi` family of modal
text editing, `fi` hopes to establish a simpler series of editing operations
that can be learned with less effort. By focusing on the composability of such
operations, `fi` aims to preserve the power of modal text editing while
improving on its traditional learning curve and complexity.

Finally, the focus on *intuitive* and *comfortable* editing may improve a
developer's focus on problem-solving rather than text editing. When large and
complex editing operations can be broken down into a series of instinctive and
interactive actions, the developer may find less context switching and mental
distraction from figuring out *what* needs to be done rather than figuring out
*how* such edits can be accomplished.

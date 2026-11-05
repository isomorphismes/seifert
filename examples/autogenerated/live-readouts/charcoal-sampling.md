# Readout surface sampling correction

Run41968030865 at source379195711e7bff028e3d9109c2d546f4bdc29320 failed the new
surface assertion, after unchanged endpoint mathematics passed. Its first
sample used the first row of a 110px crop. At width576 the actual top panel is
108.8px tall, so that row belongs to the scene background rather than the panel.

Read the actual top-left framebuffer pixel (0, HEIGHT-1) directly for the exact
47/47/47 panel requirement. Keep the independent scene-margin33/33/33 sample
and all existing visible-ink, motion, invalid-input, reopen and resize checks.
The original failed run remains evidence of the mistaken crop assumption.
No palette requirement or functional assertion is weakened.

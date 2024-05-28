import React from "react";

const AboutUs: React.FC = () => (
  <div className="p-8 pt-20 rounded-lg max-w-3xl mx-auto">
    <h1 className="text-4xl font-bold text-center mb-6">About Us</h1>
    <p className="text-lg leading-relaxed mb-4 text-center">
      Welcome to <span className="font-bold text-blue-600">Arka Linux GUI</span>
      , your gateway to an efficient and powerful Arch-based Linux experience.
      Our mission is to provide a user-friendly and robust environment that
      leverages the strengths of Arch Linux while offering a polished graphical
      interface.
    </p>
    <p className="text-lg leading-relaxed mb-4 text-center">
      At <span className="font-bold text-blue-600">Arka Linux GUI</span>, we
      believe in the power of simplicity and customization. Whether you are a
      seasoned Linux user or a newcomer, our distribution aims to deliver an
      experience that is both accessible and deeply customizable to suit your
      needs.
    </p>
  </div>
);

export default AboutUs;

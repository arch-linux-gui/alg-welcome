import React from "react";

const faqs = [
  {
    question: "How do I use this application?",
    answer:
      "This application is designed to help you manage your Arch-based Linux distribution. Navigate through the screens to perform various setup tasks.",
  },
  {
    question: "How can I toggle the theme?",
    answer:
      "Use the theme toggle button on the bottom right corner of the Welcome screen to switch between light and dark modes.",
  },
  {
    question: "What does the Country Selection screen do?",
    answer:
      "The Country Selection screen allows you to select countries and protocols to update the Arch mirrors based on your preferences.",
  },
];

const FAQ: React.FC = () => {
  return (
    <div className="p-10">
      <h2 className="text-2xl font-bold mb-4 mt-8">
        Frequently Asked Questions
      </h2>
      {faqs.map((faq, index) => (
        <div key={index} className="mb-4 text-wrap">
          <h3 className="text-xl font-semibold">
            Q.{index + 1}: {faq.question}
          </h3>
          <p className="text-base">A: {faq.answer}</p>
        </div>
      ))}
    </div>
  );
};

export default FAQ;
